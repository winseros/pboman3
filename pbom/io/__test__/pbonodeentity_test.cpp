#include <gtest/gtest.h>
#include "io/pbonodeentity.h"

namespace pboman3::io::test {
    TEST(PboNodeEntityTest, Ctor_Functional) {
        const PboNodeEntity entry("some-file", PboPackingMethod::Packed, 1, 2, 3, 4);
        ASSERT_EQ(entry.fileName(), "some-file");
        ASSERT_EQ(entry.packingMethod(), PboPackingMethod::Packed);
        ASSERT_EQ(entry.originalSize(), 1);
        ASSERT_EQ(entry.reserved(), 2);
        ASSERT_EQ(entry.timestamp(), 3);
        ASSERT_EQ(entry.dataSize(), 4);
    }

    TEST(PboNodeEntityTest, IsBoundary_Functional) {
        const PboNodeEntity entry = PboNodeEntity::makeBoundary();
        ASSERT_TRUE(entry.isBoundary());
        ASSERT_FALSE(entry.isContent());
    }

    TEST(PboNodeEntityTest, IsSignature_Functional) {
        const PboNodeEntity entry = PboNodeEntity::makeSignature();
        ASSERT_TRUE(entry.isSignature());
        ASSERT_FALSE(entry.isContent());
    }

    // ReSharper disable once CppInconsistentNaming
    class PboNodeEntityTest_IsContent : public testing::TestWithParam<PboPackingMethod> {
    };

    TEST_P(PboNodeEntityTest_IsContent, Functional) {
        const PboNodeEntity entry("some-file", GetParam(), 100, 0, 0, 100);
        ASSERT_FALSE(entry.isSignature());
        ASSERT_FALSE(entry.isBoundary());
        ASSERT_TRUE(entry.isContent());
    }

    INSTANTIATE_TEST_SUITE_P(IsContent, PboNodeEntityTest_IsContent,
                             testing::Values(PboPackingMethod::Packed, PboPackingMethod::Uncompressed));

    TEST(PboNodeEntityTest, Size_Functional) {
        const PboNodeEntity entry("some-file", PboPackingMethod::Packed, 1, 2, 3, 4);
        ASSERT_EQ(entry.size(), entry.fileName().size() + 21);
    }

    TEST(PboNodeEntityTest, IsCompressed_Functional) {
        const PboNodeEntity entry1("some-file", PboPackingMethod::Packed, 1, 2, 3, 4);
        ASSERT_TRUE(entry1.isCompressed());
        const PboNodeEntity entry2("some-file", PboPackingMethod::Uncompressed, 1, 2, 3, 1);
        ASSERT_FALSE(entry2.isCompressed());
    }
}
