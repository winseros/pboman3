#include <gtest/gtest.h>
#include "io/pboheader.h"

namespace pboman3::io::test {
    TEST(PboHeaderTest, Ctor_Functional) {
        const PboHeader header("name1", "value1");
        ASSERT_EQ(header.name, "name1");
        ASSERT_EQ(header.value, "value1");

        ASSERT_FALSE(header.isBoundary());
    }

    TEST(PboHeaderTest, IsBoundary_Functional) {
        const PboHeader header = PboHeader::makeBoundary();
        ASSERT_TRUE(header.isBoundary());
    }
}
