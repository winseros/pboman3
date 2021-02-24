#include "model/pbopath.h"
#include <gtest/gtest.h>

namespace pboman3::test {
    TEST(PboPath, Ctor_Initializes_Empty_Path) {
        const PboPath p;
        ASSERT_EQ(p.length(), 0);
    }

    TEST(PboPath, Ctor_Initializes_Path_From_Args) {
        const PboPath p{"p1", "p2", "p3"};

        ASSERT_EQ(p.length(), 3);
        ASSERT_EQ(p.at(0), "p1");
        ASSERT_EQ(p.at(1), "p2");
        ASSERT_EQ(p.at(2), "p3");
    }

    TEST(PboPath, Ctor_Initializes_Path_From_String) {
        const PboPath p1("p1/p2/p3");

        ASSERT_EQ(p1.length(), 3);
        ASSERT_EQ(p1.at(0), "p1");
        ASSERT_EQ(p1.at(1), "p2");
        ASSERT_EQ(p1.at(2), "p3");

        const PboPath p2("e1\\e2\\e3");

        ASSERT_EQ(p2.length(), 3);
        ASSERT_EQ(p2.at(0), "e1");
        ASSERT_EQ(p2.at(1), "e2");
        ASSERT_EQ(p2.at(2), "e3");
    }

    TEST(PboPath, Ctor_Initializes_Path_From_Url) {
        const PboPath p1(QUrl("/p1/p2/p3?abcd=1"));

        ASSERT_EQ(p1.length(), 3);
        ASSERT_EQ(p1.at(0), "p1");
        ASSERT_EQ(p1.at(1), "p2");
        ASSERT_EQ(p1.at(2), "p3");

        const PboPath p2(QUrl("p1/p2/p3?abcd=1"));

        ASSERT_EQ(p2.length(), 3);
        ASSERT_EQ(p2.at(0), "p1");
        ASSERT_EQ(p2.at(1), "p2");
        ASSERT_EQ(p2.at(2), "p3");
    }
}
