#include <gtest/gtest.h>
#include "io/pboheaderentity.h"

namespace pboman3::io::test {
    TEST(PboHeaderEntityTest, Ctor_Functional) {
        const PboHeaderEntity header("name1", "value1");
        ASSERT_EQ(header.name, "name1");
        ASSERT_EQ(header.value, "value1");

        ASSERT_FALSE(header.isBoundary());
    }

    TEST(PboHeaderEntityTest, IsBoundary_Functional) {
        const PboHeaderEntity header = PboHeaderEntity::makeBoundary();
        ASSERT_TRUE(header.isBoundary());
    }
}
