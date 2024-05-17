#include <gtest/gtest.h>
#include <QObject>
#include "util/numbers.h"

namespace pboman3::util::test {
    struct NumberUtilsTestParam {
        qsizetype value;
        int expectedResult;
    };

    class NumberUtilsTest : public ::testing::TestWithParam<NumberUtilsTestParam> {
    };

    TEST_P(NumberUtilsTest, GetNumberOfDigits_Returns_Correct_Result) {
        const auto param = GetParam();
        const auto result = NumberUtils::GetNumberOfDigits(param.value);
        ASSERT_EQ(result, param.expectedResult);
    }

    INSTANTIATE_TEST_SUITE_P(TestSuite, NumberUtilsTest, ::testing::Values(
            NumberUtilsTestParam{0, 1},
            NumberUtilsTestParam{1, 1},
            NumberUtilsTestParam{10, 2},
            NumberUtilsTestParam{99, 2},
            NumberUtilsTestParam{100, 3},
            NumberUtilsTestParam{101, 3}
    ));
}