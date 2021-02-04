#include <gtest/gtest.h>
#include "model/pboentry.h"

namespace pboman3::test {
    TEST(PboEntryTest, Ctor_Functional) {
        const PboEntry entry("some-file", PboPackingMethod::Packed, 1, 2, 3, 4);
        ASSERT_EQ(entry.fileName, "some-file");
        ASSERT_EQ(entry.packingMethod, PboPackingMethod::Packed);
        ASSERT_EQ(entry.originalSize, 1);
        ASSERT_EQ(entry.reserved, 2);
        ASSERT_EQ(entry.timestamp, 3);
        ASSERT_EQ(entry.dataSize, 4);
    }

    TEST(PboEntryTest, IsBoundary_Functional) {
        const PboEntry entry = PboEntry::makeBoundary();
        ASSERT_TRUE(entry.isBoundary());
        ASSERT_FALSE(entry.isContent());
    }

    TEST(PboEntryTest, IsSignature_Functional) {
        const PboEntry entry = PboEntry::makeSignature();
        ASSERT_TRUE(entry.isSignature());
        ASSERT_FALSE(entry.isContent());
    }

    class PboEntryTest_IsContent : public testing::TestWithParam<PboPackingMethod> {
    };

    TEST_P(PboEntryTest_IsContent, Functional) {
        const PboEntry entry("some-file", GetParam(), 100, 0, 0, 100);
        ASSERT_FALSE(entry.isSignature());
        ASSERT_FALSE(entry.isBoundary());
        ASSERT_TRUE(entry.isContent());
    }

    INSTANTIATE_TEST_SUITE_P(IsContent, PboEntryTest_IsContent,
                             testing::Values(PboPackingMethod::Packed, PboPackingMethod::Uncompressed));

    TEST(PboEntryTest, Size_Functional) {
        const PboEntry entry("some-file", PboPackingMethod::Packed, 1, 2, 3, 4);
        ASSERT_EQ(entry.size(), entry.fileName.size() + 21);
    }

    TEST(PboEntryTest, IsCompressed_Functional) {
        const PboEntry entry1("some-file", PboPackingMethod::Packed, 1, 2, 3, 4);
        ASSERT_TRUE(entry1.isCompressed());
        const PboEntry entry2("some-file", PboPackingMethod::Uncompressed, 1, 2, 3, 1);
        ASSERT_FALSE(entry2.isCompressed());
    }
}
