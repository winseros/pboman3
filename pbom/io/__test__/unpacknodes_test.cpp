#pragma once

#include "io/unpacknodes.h"
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QSharedPointer>
#include <gtest/gtest.h>
#include "io/bs/fsrawbinarysource.h"
#include "util/exception.h"

namespace pboman3::test {
    TEST(UnpackNodesTest, UnpackTo_Extracts_Nodes_To_File_System) {
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

        UnpackNodes unpack;
        unpack.unpackTo(dir.path(), &tree, list, []() { return false; });

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

    TEST(UnpackNodesTest, UnpackTo_Extracts_Contents) {
        const QTemporaryDir dir;

        QTemporaryFile t1;
        t1.open();
        t1.write(QByteArray(10, 1));
        t1.close();

        PboNode tree("tree.pbo", PboNodeType::Container, nullptr);
        PboNode* e1 = tree.createHierarchy(PboPath("f1/f2/f3/e1.txt"));
        e1->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(t1.fileName()));
        e1->binarySource->open();

        UnpackNodes unpack;
        unpack.unpackTo(dir.path(), tree.get(PboPath("f1/f2")), QList({e1}), []() { return false; });

        const QString filePath = dir.filePath("f3/e1.txt");
        ASSERT_TRUE(QFile::exists(filePath));

        QFile f1(filePath);
        f1.open(QIODeviceBase::ReadOnly);
        ASSERT_EQ(f1.readAll(), QByteArray(10, 1));
        f1.close();
    }

    TEST(UnpackNodesTest, UnpackTo_Throws_If_Root_Is_Invalid) {
        const QTemporaryDir dir;

        PboNode tree("tree.pbo", PboNodeType::Container, nullptr);
        PboNode* e1 = tree.createHierarchy(PboPath("f1/e1.txt"));
        tree.createHierarchy(PboPath("f2/e2.txt"));

        UnpackNodes unpack;
        ASSERT_THROW(unpack.unpackTo(dir.path(), tree.get(PboPath("f2")), QList({ e1 }), []() { return false; }), InvalidOperationException);
    }
}
