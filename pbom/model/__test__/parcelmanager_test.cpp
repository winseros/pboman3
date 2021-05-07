#include "model/parcelmanager.h"
#include <QTemporaryFile>
#include <gtest/gtest.h>
#include "io/bs/fsrawbinarysource.h"
#include "io/bs/pbobinarysource.h"
#include "model/pbonode.h"

namespace pboman3::test {
    TEST(ParcelManagerTest, PackTree_Creates_Parcel) {
        //the pbo nodes tree
        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(PboPath("e1"));
        root.addEntry(PboPath("f2/e2"));
        root.addEntry(PboPath("f2/e3"));
        root.addEntry(PboPath("f3/e1"));

        //set binary sources to nodes
        QTemporaryFile t;
        t.open();
        t.close();
        root.child(0)->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(t.fileName()));
        root.child(1)->child(0)->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(t.fileName()));
        root.child(1)->child(1)->binarySource = QSharedPointer<BinarySource>(
            new PboBinarySource(t.fileName(), PboDataInfo(10, 20, 30)));
        root.child(2)->child(0)->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(t.fileName()));

        //test the method
        const QList<PboPath> paths{PboPath("e1"), PboPath("f2/e2"), PboPath("f2"), PboPath("f3/e1")};
        PboParcel parcel = ParcelManager().packTree(&root, paths);

        //verify the results
        ASSERT_EQ(parcel.length(), 4);

        ASSERT_EQ(parcel.at(0).path, "f2/e2");
        ASSERT_EQ(parcel.at(0).file, t.fileName());
        ASSERT_EQ(parcel.at(0).dataOffset, 0);
        ASSERT_EQ(parcel.at(0).originalSize, -1);
        ASSERT_EQ(parcel.at(0).dataSize, -1);

        ASSERT_EQ(parcel.at(1).path, "f2/e3");
        ASSERT_EQ(parcel.at(1).file, t.fileName());
        ASSERT_EQ(parcel.at(1).dataOffset, 30);
        ASSERT_EQ(parcel.at(1).originalSize, 10);
        ASSERT_EQ(parcel.at(1).dataSize, 20);

        ASSERT_EQ(parcel.at(2).path, "e1");
        ASSERT_EQ(parcel.at(2).file, t.fileName());
        ASSERT_EQ(parcel.at(2).dataOffset, 0);
        ASSERT_EQ(parcel.at(2).originalSize, -1);
        ASSERT_EQ(parcel.at(2).dataSize, -1);

        ASSERT_EQ(parcel.at(3).path, "e1");
        ASSERT_EQ(parcel.at(3).file, t.fileName());
        ASSERT_EQ(parcel.at(3).dataOffset, 0);
        ASSERT_EQ(parcel.at(3).originalSize, -1);
        ASSERT_EQ(parcel.at(3).dataSize, -1);
    }

    TEST(ParcelManagerTest, UnpackTree_Unpacks_Parcel) {
        QTemporaryFile temp;
        temp.open();
        temp.close();

        PboParcel parcel;
        parcel.append(PboParcelItem{"e1.txt", temp.fileName(), 0, -1, -1});
        parcel.append(PboParcelItem{"f2/e1.txt", temp.fileName(), 1, 2, 3});
        QByteArray data = parcel.serialize();

        PboNode root("file.pbo", PboNodeType::Container, nullptr, nullptr);
        ParcelManager().unpackTree(&root, parcel, nullptr);

        ASSERT_EQ(root.childCount(), 2);
        ASSERT_EQ(root.child(0)->title(), "e1.txt");
        ASSERT_TRUE(dynamic_cast<FsRawBinarySource*>(root.child(0)->binarySource.get()));

        const auto* pboBs = dynamic_cast<PboBinarySource*>(root.child(1)->child(0)->binarySource.get());
        ASSERT_TRUE(pboBs);
        ASSERT_EQ(pboBs->getInfo().dataOffset, 1);
        ASSERT_EQ(pboBs->getInfo().dataSize, 2);
        ASSERT_EQ(pboBs->getInfo().originalSize, 3);
    }
}
