#include <QDir>
#include <QTemporaryFile>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "io/bb/tempbackend.h"
#include <QTemporaryDir>
#include <QUuid>
#include "io/bs/fsrawbinarysource.h"

namespace pboman3::io::test {
    TEST(TempBackendTest, HddSync_Creates_Files_On_Disk) {
        //dummy files
        QTemporaryFile f1;
        f1.open();
        f1.write(QByteArray("some text data 1"));
        f1.close();

        QTemporaryFile f2;
        f2.open();
        f2.write(QByteArray("some text data 2"));
        f2.close();

        //nodes to sync
        PboNode root("root", PboNodeType::Container, nullptr);
        PboNode* e1 = root.createHierarchy(PboPath("file1.txt"));
        PboNode* e2 = root.createHierarchy(PboPath("folder1/file2.txt"));

        e1->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(f1.fileName()));
        e1->binarySource->open();
        e2->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(f2.fileName()));
        e2->binarySource->open();

        //the object tested
        const QTemporaryDir dir;
        const TempBackend be(QDir(dir.path()));
        const QList<QUrl> sync = be.hddSync(QList({e1, e2, root.at(0)}), []() { return false; });

        //check the result
        ASSERT_EQ(sync.length(), 3);

        ASSERT_EQ(sync.at(0).toLocalFile(), dir.filePath("file1.txt"));
        ASSERT_EQ(sync.at(1).toLocalFile(), dir.filePath("folder1/file2.txt"));
        ASSERT_EQ(sync.at(2).toLocalFile(), dir.filePath("folder1"));

        //ensure the files have their content
        QFile c1(sync.at(0).toLocalFile());
        c1.open(QIODeviceBase::ReadOnly);
        ASSERT_EQ(c1.readAll(), "some text data 1");
        c1.close();

        QFile c2(sync.at(1).toLocalFile());
        c2.open(QIODeviceBase::ReadOnly);
        ASSERT_EQ(c2.readAll(), "some text data 2");
        c2.close();
    }

    TEST(TempBackendTest, Dtor_Cleans_Disk_After_Itself) {
        //dummy files
        QTemporaryFile f1;
        f1.open();
        f1.write(QByteArray("some text data 1"));
        f1.close();

        QTemporaryFile f2;
        f2.open();
        f2.write(QByteArray("some text data 2"));
        f2.close();

        //nodes to sync
        PboNode root("root", PboNodeType::Container, nullptr);
        PboNode* e1 = root.createHierarchy(PboPath("file1.txt"));
        PboNode* e2 = root.createHierarchy(PboPath("folder1/file2.txt"));

        e1->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(f1.fileName()));
        e1->binarySource->open();
        e2->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(f2.fileName()));
        e2->binarySource->open();

        //the object tested
        const QTemporaryDir dir;
        const auto be = new TempBackend(QDir(dir.path()));
        be->hddSync(QList({e1, e2, root.at(0)}), []() { return false; });

        //must clean up
        delete be;

        ASSERT_EQ(QDir(dir.path()).entryList().count(), 0);
    }

    TEST(TempBackendTest, Clear_Cleans_Up_If_Needed) {
        //dummy files
        QTemporaryFile f1;
        f1.open();
        f1.write(QByteArray("some text data 1"));
        f1.close();

        //nodes to sync
        PboNode root("root", PboNodeType::Container, nullptr);
        PboNode* e1 = root.createHierarchy(PboPath("file1.txt"));

        e1->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(f1.fileName()));
        e1->binarySource->open();

        //the object tested
        const QTemporaryDir dir;
        const TempBackend be(QDir(dir.path()));
        const QList<QUrl> sync = be.hddSync(QList({e1}), []() { return false; });

        //call the method
        be.clear(e1);

        //check
        ASSERT_FALSE(QFile::exists(sync.at(0).path()));
    }

    TEST(TempBackendTest, Clear_Does_Not_Clean_Up) {
        //nodes to sync
        PboNode root("root", PboNodeType::Container, nullptr);
        const PboNode* e1 = root.createHierarchy(PboPath("file1.txt"));

        //the object tested
        const QTemporaryDir dir;
        const TempBackend be(QDir(dir.path()));

        //call the method
        ASSERT_NO_THROW(be.clear(e1));
    }
}
