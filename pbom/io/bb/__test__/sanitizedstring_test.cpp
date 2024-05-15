#include "io/bb/sanitizedstring.h"
#include <QString>
#include <gtest/gtest.h>

namespace pboman3::io::test {
    struct CtorParam {
        const QString sourceText;
        const QString expectedText;
    };

    class CtorTest : public testing::TestWithParam<CtorParam> {
    };

    TEST_P(CtorTest, Deals_With_Restricted_Characters) {
        SanitizedString ss(GetParam().sourceText);
        ASSERT_EQ(static_cast<QString>(ss), GetParam().expectedText);
    }

    INSTANTIATE_TEST_SUITE_P(SanitizedStringTest, CtorTest, testing::Values(
                                 CtorParam{"\t1\t", "%91%9"},
                                 CtorParam{"?1?", "%3f1%3f"},
                                 CtorParam{"*1*", "%2a1%2a"},
                                 CtorParam{"1///", "1%2f%2f%2f"},
                                 CtorParam{"\\2", "%5c2"}
                             ));
}
