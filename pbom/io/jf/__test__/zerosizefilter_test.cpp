#include "io/jf/zerosizefilter.h"
#include <gtest/gtest.h>

namespace pboman3::io::test {
    TEST(ZeroSizeFilterTest, IsJunk_Returns_False) {
        const ZeroSizeFilter filter;

        const PboNodeEntity e1 {"f1", PboPackingMethod::Uncompressed, 1, 0, 0, 1};
        const auto isJunk = filter.IsJunk(&e1);
        ASSERT_FALSE(isJunk);
    }

    TEST(ZeroSizeFilterTest, IsJunk_Returns_True) {
        const ZeroSizeFilter filter;

        const PboNodeEntity e1 {"f1", PboPackingMethod::Uncompressed, 0, 0, 0, 1};

        auto isJunk = filter.IsJunk(&e1);
        ASSERT_TRUE(isJunk);

        const PboNodeEntity e2 {"f1", PboPackingMethod::Uncompressed, 1, 0, 0, 0};
        isJunk = filter.IsJunk(&e2);
        ASSERT_TRUE(isJunk);
    }
}
