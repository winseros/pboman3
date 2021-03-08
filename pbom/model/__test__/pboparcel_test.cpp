#include "model/pboparcel.h"
#include <gtest/gtest.h>

namespace pboman3::test {
    TEST(PboParcelTest, Serialization_And_Deserialization_Work) {
        PboParcel source;
        source.append(PboParcelItem{"pbo/entry/path1", "pbo/file/path1", 10, 20, 30});
        source.append(PboParcelItem{"pbo/entry/path2", "pbo/file/path2", 40, 50, 60});
        const QByteArray serialized = source.serialize();

        const PboParcel copy = PboParcel::deserialize(serialized);
        ASSERT_EQ(copy.length(), 2);

        ASSERT_EQ(copy.at(0).path, source.at(0).path);
        ASSERT_EQ(copy.at(0).file, source.at(0).file);
        ASSERT_EQ(copy.at(0).dataOffset, source.at(0).dataOffset);
        ASSERT_EQ(copy.at(0).originalSize, source.at(0).originalSize);
        ASSERT_EQ(copy.at(0).dataSize, source.at(0).dataSize);

        ASSERT_EQ(copy.at(1).path, source.at(1).path);
        ASSERT_EQ(copy.at(1).file, source.at(1).file);
        ASSERT_EQ(copy.at(1).dataOffset, source.at(1).dataOffset);
        ASSERT_EQ(copy.at(1).originalSize, source.at(1).originalSize);
        ASSERT_EQ(copy.at(1).dataSize, source.at(1).dataSize);
    }
}
