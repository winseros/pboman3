#include "io/bb/unpackbackend.h"
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QSharedPointer>
#include <gtest/gtest.h>
#include "io/bs/fsrawbinarysource.h"
#include "io/bs/pbobinarysource.h"
#include "util/exception.h"

namespace pboman3::io::test {
    TEST(UnpackBackendTest, UnpackSync_Extracts_Nodes_To_File_System) {
        const QTemporaryDir dir;

        QTemporaryFile t1;
        t1.open();
        t1.write(QByteArray(10, 1));
        t1.close();

        QTemporaryFile t2;
        t2.open();
        t2.write(QByteArray(10, 2));
        t2.close();

        QTemporaryFile t3;
        t3.open();
        t3.write(QByteArray(10, 3));
        t3.close();

        QTemporaryFile t4;
        t4.open();
        t4.write(QByteArray(10, 4));
        t4.close();

        //should be extracted
        PboNode tree("tree.pbo", PboNodeType::Container, nullptr);
        PboNode* e1 = tree.createHierarchy(PboPath("e1.txt"));
        e1->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(t1.fileName()));
        e1->binarySource->open();
        PboNode* e2 = tree.createHierarchy(PboPath("f1/e2.txt"));
        e2->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(t2.fileName()));
        e2->binarySource->open();
        PboNode* e3 = tree.createHierarchy(PboPath("f1/e3.txt"));
        e3->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(t3.fileName()));
        e3->binarySource->open();
        PboNode* e4 = tree.createHierarchy(PboPath("f2/e4.txt"));
        e4->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(t4.fileName()));
        e4->binarySource->open();

        //should not be extracted
        tree.createHierarchy(PboPath("e0.txt"));
        tree.createHierarchy(PboPath("f2/e0.txt"));

        const QList list({e1, e4, tree.get(PboPath("f1"))});

        UnpackBackend unpack(QDir(dir.path()));
        unpack.unpackSync(&tree, list, []() { return false; });

        constexpr auto f = QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files;
        constexpr auto s = QDir::DirsFirst | QDir::Name;

        ASSERT_EQ(
            QDir(dir.path()).entryList(f, s),
            QStringList({ "f1", "f2", "e1.txt"}));

        ASSERT_EQ(
            QDir(dir.path() + QDir::separator() + "f1").entryList(f, s),
            QStringList({ "e2.txt", "e3.txt" }));

        ASSERT_EQ(
            QDir(dir.path() + QDir::separator() + "f2").entryList(f, s),
            QStringList({ "e4.txt" }));
    }

    TEST(UnpackBackendTest, UnpackSync_Extracts_Raw_Contents) {
        const QTemporaryDir dir;

        QTemporaryFile t1;
        t1.open();
        t1.write(QByteArray(10, 1));
        t1.close();

        PboNode tree("tree.pbo", PboNodeType::Container, nullptr);
        PboNode* e1 = tree.createHierarchy(PboPath("f1/f2/f3/f4/f5/e1.txt"));
        e1->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(t1.fileName()));
        e1->binarySource->open();

        UnpackBackend unpack(QDir(dir.path()));
        unpack.unpackSync(tree.get(PboPath("f1/f2")), QList({e1}), []() { return false; });

        const QString filePath = dir.filePath("f3/f4/f5/e1.txt");
        ASSERT_TRUE(QFile::exists(filePath));

        QFile f1(filePath);
        f1.open(QIODeviceBase::ReadOnly);
        ASSERT_EQ(f1.readAll(), QByteArray(10, 1));
        f1.close();
    }

    TEST(UnpackBackendTest, UnpackSync_Extracts_Lzh_Contents) {
        const QTemporaryDir dir;

        QTemporaryFile t1;
        t1.open();
        t1.write(QByteArray(
            "\xFFLorem Ip\xFFsum is s\xFFimply du\xFFmmy text\xFF of the \xFFprinting\xFF and typ\xEF"
            "eset\x10\x02ind?ustry.\xFC\x1B\x00\x00", 84));
        t1.close();

        PboNode tree("tree.pbo", PboNodeType::Container, nullptr);
        PboNode* e1 = tree.createHierarchy(PboPath("f1/f2/f3/f4/f5/e1.txt"));
        constexpr PboDataInfo dataInfo{74, 84, 0, 0, true}; //the file is marked as compressed
        e1->binarySource = QSharedPointer<BinarySource>(new PboBinarySource(t1.fileName(), dataInfo));
        e1->binarySource->open();

        UnpackBackend unpack(QDir(dir.path()));
        unpack.unpackSync(tree.get(PboPath("f1/f2")), QList({e1}), []() { return false; });

        const QString filePath = dir.filePath("f3/f4/f5/e1.txt");
        ASSERT_TRUE(QFile::exists(filePath));

        QFile f1(filePath);
        f1.open(QIODeviceBase::ReadOnly);
        ASSERT_EQ(f1.readAll(), QString("Lorem Ipsum is simply dummy text of the printing and typesetting industry."));
        f1.close();
    }

    TEST(UnpackBackendTest, UnpackSync_Throws_If_Root_Is_Invalid) {
        const QTemporaryDir dir;

        PboNode tree("tree.pbo", PboNodeType::Container, nullptr);
        PboNode* e1 = tree.createHierarchy(PboPath("f1/e1.txt"));
        tree.createHierarchy(PboPath("f2/e2.txt"));

        UnpackBackend unpack(QDir(dir.path()));
        ASSERT_THROW(unpack.unpackSync(tree.get(PboPath("f2")), QList({ e1 }), []() { return false; }),
                     InvalidOperationException);
    }
}
