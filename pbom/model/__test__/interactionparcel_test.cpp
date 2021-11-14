#include "model/interactionparcel.h"
#include <QTemporaryFile>
#include <gtest/gtest.h>
#include "io/bs/fsrawbinarysource.h"
#include "io/bs/pbobinarysource.h"
#include "domain/pbonode.h"
#include "domain/pbopath.h"

namespace pboman3::model::test {
    TEST(NodeDescriptorTest, Ctor_Initializes_Fields) {
        QTemporaryFile t;
        t.open();
        t.close();

        const QSharedPointer<BinarySource> bs(new FsRawBinarySource(t.fileName()));
        const PboPath pt("f1/f2/e1");
        const NodeDescriptor d(bs, pt);

        ASSERT_EQ(d.binarySource(), bs);
        ASSERT_EQ(d.path(), pt);
    }

    TEST(NodeDescriptorTest, SetCompressed_Changes_Binary_Source) {
        QTemporaryFile t;
        t.open();
        t.close();

        const QSharedPointer<BinarySource> bs(new FsRawBinarySource(t.fileName()));
        const PboPath pt("f1/f2/e1");
        NodeDescriptor d(bs, pt);

        //changed to compressed
        d.setCompressed(true);
        auto* lzh = dynamic_cast<FsLzhBinarySource*>(d.binarySource().get());
        ASSERT_TRUE(lzh);
        ASSERT_EQ(lzh->path(), t.fileName());

        //changed to uncompressed
        d.setCompressed(false);
        auto* raw = dynamic_cast<FsRawBinarySource*>(d.binarySource().get());
        ASSERT_TRUE(raw);
        ASSERT_EQ(raw->path(), t.fileName());
    }

    TEST(NodeDescriptorsTest, PackTree_Creates_Descriptors) {
        //the pbo nodes tree
        PboNode root("file-name", PboNodeType::Container, nullptr);
        PboNode* e1 = root.createHierarchy(PboPath("e1"));
        PboNode* e2 = root.createHierarchy(PboPath("f2/e2"));
        PboNode* e3 = root.createHierarchy(PboPath("f2/e3"));
        PboNode* e4 = root.createHierarchy(PboPath("f3/e1"));

        //set binary sources to nodes
        QTemporaryFile t;
        t.open();
        t.close();

        e1->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(t.fileName()));
        e2->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(t.fileName()));
        e3->binarySource = QSharedPointer<BinarySource>(
            new PboBinarySource(t.fileName(), PboDataInfo{10, 20, 30, 0, true}));
        e4->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(t.fileName()));

        //test the method
        const QList paths{e1, e2, root.get(PboPath("f2")), e4};
        const NodeDescriptors descriptors = NodeDescriptors::packNodes(paths);

        //verify the results
        ASSERT_EQ(descriptors.length(), 4);

        ASSERT_EQ(descriptors.at(0).path(), PboPath("f2/e2"));
        ASSERT_EQ(descriptors.at(0).binarySource(), e2->binarySource);

        ASSERT_EQ(descriptors.at(1).path(), PboPath("f2/e3"));
        ASSERT_EQ(descriptors.at(1).binarySource(), e3->binarySource);

        ASSERT_EQ(descriptors.at(2).path(), PboPath("e1"));
        ASSERT_EQ(descriptors.at(2).binarySource(), e1->binarySource);

        ASSERT_EQ(descriptors.at(3).path(), PboPath("e1"));
        ASSERT_EQ(descriptors.at(3).binarySource(), e4->binarySource);
    }

    TEST(NodeDescriptorsTest, Serialization_And_Deserialization_Work) {
        QTemporaryFile t1;
        t1.open();
        t1.close();
        QTemporaryFile t2;
        t2.open();
        t2.close();
        QTemporaryFile t3;
        t3.open();
        t3.close();

        const auto bs1 = QSharedPointer<PboBinarySource
        >(new PboBinarySource(t1.fileName(), PboDataInfo{100, 200, 300, 0, true}));
        const auto bs2 = QSharedPointer<FsRawBinarySource>(new FsRawBinarySource(t2.fileName()));
        const auto bs3 = QSharedPointer<FsLzhBinarySource>(new FsLzhBinarySource(t3.fileName()));

        NodeDescriptors source;
        source.append(NodeDescriptor(bs1, PboPath("pbo/entry/path1")));
        source.append(NodeDescriptor(bs2, PboPath("pbo/entry/path2")));
        source.append(NodeDescriptor(bs3, PboPath("pbo/entry/path3")));
        const QByteArray serialized = NodeDescriptors::serialize(source);

        const NodeDescriptors copy = NodeDescriptors::deserialize(serialized);
        ASSERT_EQ(copy.length(), 3);

        ASSERT_EQ(copy.at(0).path(), source.at(0).path());
        const auto* cs1 = dynamic_cast<PboBinarySource*>(copy.at(0).binarySource().get());
        ASSERT_TRUE(cs1);
        ASSERT_EQ(cs1->path(), bs1->path());
        ASSERT_EQ(cs1->getInfo().dataOffset, bs1->getInfo().dataOffset);
        ASSERT_EQ(cs1->getInfo().dataSize, bs1->getInfo().dataSize);
        ASSERT_EQ(cs1->getInfo().originalSize, bs1->getInfo().originalSize);
        ASSERT_EQ(cs1->getInfo().compressed, bs1->getInfo().compressed);

        ASSERT_EQ(copy.at(1).path(), source.at(1).path());
        const auto* cs2 = dynamic_cast<FsRawBinarySource*>(copy.at(1).binarySource().get());
        ASSERT_TRUE(cs2);
        ASSERT_EQ(cs2->path(), bs2->path());

        ASSERT_EQ(copy.at(2).path(), source.at(2).path());
        const auto* cs3 = dynamic_cast<FsLzhBinarySource*>(copy.at(2).binarySource().get());
        ASSERT_TRUE(cs3);
        ASSERT_EQ(cs3->path(), bs3->path());
    }

    TEST(NodeDescriptorsTest, Deserialization_Opens_Binary_Sources) {
        QTemporaryFile t1;
        t1.open();
        t1.close();

        const auto bs1 = QSharedPointer<FsRawBinarySource>(new FsRawBinarySource(t1.fileName()));

        NodeDescriptors source;
        source.append(NodeDescriptor(bs1, PboPath("pbo/entry/path1")));
        const QByteArray serialized = NodeDescriptors::serialize(source);

        const NodeDescriptors copy = NodeDescriptors::deserialize(serialized);
        ASSERT_EQ(copy.length(), 1);

        auto* cs1 = dynamic_cast<FsRawBinarySource*>(copy.at(0).binarySource().get());
        ASSERT_TRUE(cs1);
        ASSERT_TRUE(cs1->isOpen());
    }
}
