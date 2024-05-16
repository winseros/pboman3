#include "io/bb/sanitizedstring.h"
#include <QString>
#include <QRegularExpression>
#include <gtest/gtest.h>

namespace pboman3::io::test {
    struct SanitizedStringTestParam {
        const QString sourceText;
        const QString expectedTextOrPattern;
    };

    class SanitizedStringRestrictedCharactersTest : public testing::TestWithParam<SanitizedStringTestParam> {
    };

    TEST_P(SanitizedStringRestrictedCharactersTest, Deals_With_Restricted_Characters) {
        SanitizedString ss(GetParam().sourceText);
        ASSERT_EQ(static_cast<QString>(ss), GetParam().expectedTextOrPattern);
    }

    INSTANTIATE_TEST_SUITE_P(TestSuite, SanitizedStringRestrictedCharactersTest, testing::Values(
            SanitizedStringTestParam{"\t1\t", "%91%9"},
            SanitizedStringTestParam{"?1?", "%3f1%3f"},
            SanitizedStringTestParam{"*1*", "%2a1%2a"},
            SanitizedStringTestParam{"1///", "1%2f%2f%2f"},
            SanitizedStringTestParam{"\\2", "%5c2"}
    ));

    class SanitizedStringRestrictedKeywordsTest : public testing::TestWithParam<SanitizedStringTestParam> {
    };

    TEST_P(SanitizedStringRestrictedKeywordsTest, Deals_With_Restricted_Keywords) {
        SanitizedString ss(GetParam().sourceText);
        QRegularExpression re(GetParam().expectedTextOrPattern);
        const QRegularExpressionMatch match = re.match(static_cast<QString>(ss));
        ASSERT_TRUE(match.hasMatch());
    }

    INSTANTIATE_TEST_SUITE_P(TestSuite, SanitizedStringRestrictedKeywordsTest, testing::Values(
        SanitizedStringTestParam{"COn", "^COn-\\d{1,4}"},
        SanitizedStringTestParam{"COM1", "^COM1-\\d{1,4}"},
        SanitizedStringTestParam{"lPt2", "^lPt2-\\d{1,4}"},
        SanitizedStringTestParam{"NUL", "^NUL-\\d{1,4}"}
    ));
}
