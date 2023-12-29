#include <QDir>
#include <QTemporaryDir>
#include <gtest/gtest.h>
#include "ui/fscollector.h"

namespace pboman3::ui::test {
    TEST(FsCollectorTest, CollectFiles_Finds_Files_In_All_Folders) {
        const QString d1 = "f1";
        const QString d11 = d1 + QDir::separator() + "f11";
        const QString d12 = d1 + QDir::separator() + "f12";
        const QString d2 = "f2";

        const QTemporaryDir temp;
        const QDir tempDir(temp.path());

        ASSERT_TRUE(tempDir.mkpath(d11));
        ASSERT_TRUE(tempDir.mkpath(d12));
        ASSERT_TRUE(tempDir.mkpath(d2));

        QFile f1(tempDir.absoluteFilePath("f1.txt"));
        f1.open(QIODeviceBase::ReadWrite);
        f1.close();

        QFile f2(tempDir.absoluteFilePath(d1 + QDir::separator() + "f2.txt"));
        f2.open(QIODeviceBase::ReadWrite);
        f2.close();

        QFile f3(tempDir.absoluteFilePath(d11 + QDir::separator() + "f3.txt"));
        f3.open(QIODeviceBase::ReadWrite);
        f3.close();

        QFile f4(tempDir.absoluteFilePath(d11 + QDir::separator() + "f4.txt"));
        f4.open(QIODeviceBase::ReadWrite);
        f4.close();

        QFile f5(tempDir.absoluteFilePath(d2 + QDir::separator() + "f5.txt"));
        f5.open(QIODeviceBase::ReadWrite);
        f5.close();

        const QSharedPointer<NodeDescriptors> files = FsCollector::collectFiles(QList{
                QUrl::fromLocalFile(tempDir.filePath(d2)),
                QUrl::fromLocalFile(f1.fileName()),
                QUrl::fromLocalFile(tempDir.filePath(d1))
            }, []() { return false; });

        ASSERT_EQ(files->length(), 5);

        ASSERT_EQ(files->at(0).path(), PboPath("f1/f11/f3.txt"));
        ASSERT_EQ(files->at(0).binarySource()->path(), f3.fileName());
        ASSERT_FALSE(files->at(0).binarySource()->isOpen());

        ASSERT_EQ(files->at(1).path(), PboPath("f1/f11/f4.txt"));
        ASSERT_EQ(files->at(1).binarySource()->path(), f4.fileName());

        ASSERT_EQ(files->at(2).path(), PboPath("f1/f2.txt"));
        ASSERT_EQ(files->at(2).binarySource()->path(), f2.fileName());

        ASSERT_EQ(files->at(3).path(), PboPath("f1.txt"));
        ASSERT_EQ(files->at(3).binarySource()->path(), f1.fileName());

        ASSERT_EQ(files->at(4).path(), PboPath("f2/f5.txt"));
        ASSERT_EQ(files->at(4).binarySource()->path(), f5.fileName());
    }

    TEST(FsCollectorTest, CollectFiles_Wont_Find_Symlinks) {
        //make a temp directory with the structure
        //temp
        // |-d1
        // | |-f1.txt
        // |-d1.link
        // |-f1.txt.link

        //temp/d1
        const QTemporaryDir temp;
        const QString d1 = "d1";
        const QDir tempDir(temp.path());
        ASSERT_TRUE(tempDir.mkpath(d1));

        //temp/d1/f1.txt
        QFile f1(tempDir.absoluteFilePath(d1 + QDir::separator() + "f1.txt"));
        f1.open(QIODeviceBase::ReadWrite);
        f1.close();

        //links
        ASSERT_TRUE(QFile::link(f1.fileName(), tempDir.filePath("f1.txt.lnk")));
        ASSERT_TRUE(QFile::link(tempDir.filePath(d1), tempDir.filePath("d1.lnk")));

        //run the code
        const QSharedPointer<NodeDescriptors> files = FsCollector::collectFiles(QList{
                QUrl::fromLocalFile(tempDir.absolutePath())
            }, []() { return false; });

        //ensure symlinks were not collected
        ASSERT_EQ(files->count(), 1);
        ASSERT_EQ(files->at(0).path(), PboPath({ tempDir.dirName(), "d1", "f1.txt" }));
    }
}
