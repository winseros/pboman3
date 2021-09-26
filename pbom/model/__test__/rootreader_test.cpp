#include "model/rootreader.h"
#include <QTemporaryFile>
#include <gtest/gtest.h>
#include "io/pboheaderreader.h"
#include "io/bs/pbobinarysource.h"

namespace pboman3::test {
    TEST(RootReaderTest, InflateRoot_Functional) {
        QTemporaryFile file;
        file.open();
        file.close();

        const PboFileHeader header{
            QList<QSharedPointer<PboHeader>>(0),
            QList{
                QSharedPointer<PboEntry>(new PboEntry("f1", PboPackingMethod::Packed, 1, 2, 3, 4)),
                QSharedPointer<PboEntry>(new PboEntry("f2", PboPackingMethod::Uncompressed, 5, 6, 7, 8)),
            },
            100,
            QByteArray()
        };

        PboNode root("root", PboNodeType::Container, nullptr);
        RootReader(&header, file.fileName()).inflateRoot(&root);

        ASSERT_EQ(root.count(), 2);

        ASSERT_EQ(root.at(0)->title(), "f1");
        const auto source1 = dynamic_cast<PboBinarySource*>(root.at(0)->binarySource.get());
        ASSERT_TRUE(source1);
        ASSERT_EQ(source1->getInfo().originalSize, header.entries.at(0)->originalSize());
        ASSERT_EQ(source1->getInfo().dataSize, header.entries.at(0)->dataSize());
        ASSERT_EQ(source1->getInfo().dataOffset, header.dataBlockStart);
        ASSERT_EQ(source1->getInfo().timestamp, header.entries.at(0)->timestamp());
        ASSERT_TRUE(source1->getInfo().compressed);

        ASSERT_EQ(root.at(1)->title(), "f2");
        const auto source2 = dynamic_cast<PboBinarySource*>(root.at(1)->binarySource.get());
        ASSERT_TRUE(source2);
        ASSERT_EQ(source2->getInfo().originalSize, header.entries.at(1)->originalSize());
        ASSERT_EQ(source2->getInfo().dataSize, header.entries.at(1)->dataSize());
        ASSERT_EQ(source2->getInfo().dataOffset, header.dataBlockStart + source1->getInfo().dataSize);
        ASSERT_EQ(source2->getInfo().timestamp, header.entries.at(1)->timestamp());
        ASSERT_FALSE(source2->getInfo().compressed);
    }
}
