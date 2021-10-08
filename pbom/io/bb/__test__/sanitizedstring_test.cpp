#include "io/bb/sanitizedstring.h"
#include <QString>
#include <gtest/gtest.h>

namespace pboman3::test {
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
                                 CtorParam{"\t1\t", "%091%09"},
                                 CtorParam{"?1?", "%3F1%3F"},
                                 CtorParam{"*1*", "%2A1%2A"},
                                 CtorParam{"1.", "1%2E"}
                             ));
}
