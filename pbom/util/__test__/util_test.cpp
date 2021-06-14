#include <QString>
#include <gtest/gtest.h>
#include "util/util.h"

namespace pboman3::test {
    struct GetFileExtensionParam {
        QString fileName;
        QString expectedResult;
    };

    class UtilTest : public ::testing::TestWithParam<GetFileExtensionParam> {
    };

    TEST_P(UtilTest, GetFileExtension_Returns_Correct_Results) {
        const QString result = GetFileExtension(GetParam().fileName);
        ASSERT_EQ(result, GetParam().expectedResult);
    }

    INSTANTIATE_TEST_SUITE_P(GetFileExtension, UtilTest, ::testing::Values(
                                 GetFileExtensionParam{"1.txt", "txt"},
                                 GetFileExtensionParam{"111", ""},
                                 GetFileExtensionParam{".config", "config"},
                                 GetFileExtensionParam{"123.tar.gz", "gz"}
                             ));
}
