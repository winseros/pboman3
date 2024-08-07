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
            SanitizedStringTestParam{"\\2", "%5c2"},
            SanitizedStringTestParam{"    ", "%20%20%20%20"},
            SanitizedStringTestParam{"1111.", "1111%2e"},
            SanitizedStringTestParam{"1111 ", "1111%20"}
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
            SanitizedStringTestParam{"COM1.c", "^COM1-\\d{1,4}.c"},
            SanitizedStringTestParam{"COn", "^COn-\\d{1,4}"},
            SanitizedStringTestParam{"COM1", "^COM1-\\d{1,4}"},
            SanitizedStringTestParam{"lPt2", "^lPt2-\\d{1,4}"},
            SanitizedStringTestParam{"NUL", "^NUL-\\d{1,4}"},
            SanitizedStringTestParam{"COM6.{69D2CF90-FC33-4FB7-9A0C-EBB0F0FCB43C}", "COM6-\\d{1,4}.%7b69D2CF90-FC33-4FB7-9A0C-EBB0F0FCB43C%7d"}
    ));

    class SanitizedStringLengthTest : public testing::TestWithParam<SanitizedStringTestParam> {
    };

    TEST_P(SanitizedStringLengthTest, Deals_With_Long_Strings) {
        SanitizedString ss(GetParam().sourceText, 50);
        ASSERT_EQ(static_cast<QString>(ss), GetParam().expectedTextOrPattern);
    }

    INSTANTIATE_TEST_SUITE_P(TestSuite, SanitizedStringLengthTest, testing::Values(
            SanitizedStringTestParam{"123456789a123456789a123456789a123456789a123456789ab",
                                     "123456789a1234-d642eb4f7beba2ee9fda95f3ed39de8~37"}
    ));
}
