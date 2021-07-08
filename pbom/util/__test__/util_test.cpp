#include <QString>
#include <gtest/gtest.h>
#include "util/util.h"
#include <QDebug>

namespace pboman3::test {
    struct GetFileExtensionParam {
        QString fileName;
        QString expectedResult;
    };

    class GetFileExtensionTest : public ::testing::TestWithParam<GetFileExtensionParam> {
    };

    TEST_P(GetFileExtensionTest, GetFileExtension_Returns_Correct_Results) {
        const QString result = GetFileExtension(GetParam().fileName);
        ASSERT_EQ(result, GetParam().expectedResult);
    }

    INSTANTIATE_TEST_SUITE_P(GetFileExtension, GetFileExtensionTest, ::testing::Values(
                                 GetFileExtensionParam{"1.txt", "txt"},
                                 GetFileExtensionParam{"111", ""},
                                 GetFileExtensionParam{".config", "config"},
                                 GetFileExtensionParam{"123.tar.gz", "gz"}
                             ));

    class GetFileNameWithoutExtensionTest : public ::testing::TestWithParam<GetFileExtensionParam> {
    };

    TEST_P(GetFileNameWithoutExtensionTest, GetFileNameWithoutExtension_Returns_Correct_Results) {
        const QString result = GetFileNameWithoutExtension(GetParam().fileName);
        qDebug() << result;
        ASSERT_EQ(result, GetParam().expectedResult);
    }

    INSTANTIATE_TEST_SUITE_P(GetFileNameWithoutExtension, GetFileNameWithoutExtensionTest, ::testing::Values(
        GetFileExtensionParam{ "1.txt", "1" },
        GetFileExtensionParam{ "111", "111" },
        GetFileExtensionParam{ ".config", ".config" },
        GetFileExtensionParam{ "123.tar.gz", "123.tar" }
    ));
}
