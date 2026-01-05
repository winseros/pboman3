#include "io/bb/sanitizedpath.h"
#include <gtest/gtest.h>

namespace pboman3::io::test {
    struct SanitizedPathTestParam {
        const QString sourceText;
        const QString expectedText;
    };

    class SanitizedPathTest : public testing::TestWithParam<SanitizedPathTestParam> {
    };

    TEST_P(SanitizedPathTest, Does_Path_Sanitization) {
        SanitizedPath sp(GetParam().sourceText);
        ASSERT_EQ(static_cast<QString>(sp), GetParam().expectedText);
    }

    INSTANTIATE_TEST_SUITE_P(TestSuite, SanitizedPathTest, testing::Values(
            SanitizedPathTestParam{"",""},
            SanitizedPathTestParam{"p1","p1"},
            SanitizedPathTestParam{"p1/p2","p1\\p2"},
            SanitizedPathTestParam{"p1/p2\\p3","p1\\p2\\p3"},
            SanitizedPathTestParam{"/p1///p2/p3\\","%2fp1\\%2f%2fp2\\p3%5c"}
    ));
}