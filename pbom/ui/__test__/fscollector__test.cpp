#include <QDir>
#include <QTemporaryDir>
#include <gtest/gtest.h>
#include "ui/fscollector.h"

namespace pboman3::test {
    TEST(FsCollectorTest, CollectFiles_Finds_Files_In_All_Folders) {
        const QString d1 = "f1";
        const QString d11 = d1 + QDir::separator() + "f11";
        const QString d12 = d1 + QDir::separator() + "f12";
        const QString d2 = "f2";

        const QTemporaryDir temp;
        const QDir tempDir(temp.path());
        assert(tempDir.mkpath(d11));
        assert(tempDir.mkpath(d12));
        assert(tempDir.mkpath(d2));

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

        FsCollector collector;
        const NodeDescriptors files = collector.collectFiles(QList<QUrl>{
            QUrl::fromLocalFile(tempDir.filePath(d2)),
            QUrl::fromLocalFile(f1.fileName()),
            QUrl::fromLocalFile(tempDir.filePath(d1))
        });

        ASSERT_EQ(files.length(), 5);

        ASSERT_EQ(files[0].path(), "f1.txt");
        ASSERT_EQ(files[0].binarySource()->path(), f1.fileName());

        ASSERT_EQ(files[1].path(), "f1/f11/f3.txt");
        ASSERT_EQ(files[1].binarySource()->path(), f3.fileName());

        ASSERT_EQ(files[2].path(), "f1/f11/f4.txt");
        ASSERT_EQ(files[2].binarySource()->path(), f4.fileName());

        ASSERT_EQ(files[3].path(), "f1/f2.txt");
        ASSERT_EQ(files[3].binarySource()->path(), f2.fileName());

        ASSERT_EQ(files[4].path(), "f2/f5.txt");
        ASSERT_EQ(files[4].binarySource()->path(), f5.fileName());
    }
}
