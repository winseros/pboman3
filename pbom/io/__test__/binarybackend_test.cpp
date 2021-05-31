#include <QDir>
#include <QTemporaryFile>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "io/binarybackend.h"
#include <QUuid>

#include "io/bs/fsrawbinarysource.h"

namespace pboman3::test {
    TEST(BinaryBackendTest, HddSync_Creates_Files_On_Disk) {
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
        e2->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(f2.fileName()));

        //the object tested
        const QString name = "test_" + QUuid::createUuid().toString(QUuid::WithoutBraces);
        const BinaryBackend be(name);
        const QList<QUrl> sync = be.hddSync(QList({e1, e2, root.at(0)}), []() { return false; });

        //check the result
        ASSERT_EQ(sync.length(), 3);

        const QString expectedPath = QDir::tempPath() + "/pboman3/" + name + "/tree_/";
        ASSERT_EQ(sync.at(0).toLocalFile(), expectedPath + "file1.txt");
        ASSERT_EQ(sync.at(1).toLocalFile(), expectedPath + "folder1/file2.txt");
        ASSERT_EQ(sync.at(2).toLocalFile(), expectedPath + "folder1");

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

    TEST(BinaryBackendTest, Dtor_Cleans_Disk_After_Itself) {
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
        e2->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(f2.fileName()));

        //the object tested
        const QString name = "test_" + QUuid::createUuid().toString(QUuid::WithoutBraces);
        const auto be = new BinaryBackend(name);
        const QList<QUrl> sync = be->hddSync(QList({ e1, e2, root.at(0) }), []() { return false; });
        //must clean up
        delete be;

        ASSERT_FALSE(QDir(QDir::tempPath() + "/pboman3/" + name + "/tree_").exists());
    }
}
