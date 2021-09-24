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
                                 GetFileExtensionParam{"123.tar.gz", "gz"},
                                 GetFileExtensionParam{"abc.", ""}
                             ));

    class GetFileNameWithoutExtensionTest : public ::testing::TestWithParam<GetFileExtensionParam> {
    };

    TEST_P(GetFileNameWithoutExtensionTest, GetFileNameWithoutExtension_Returns_Correct_Results) {
        const QString result = GetFileNameWithoutExtension(GetParam().fileName);
        ASSERT_EQ(result, GetParam().expectedResult);
    }

    INSTANTIATE_TEST_SUITE_P(GetFileNameWithoutExtension, GetFileNameWithoutExtensionTest, ::testing::Values(
        GetFileExtensionParam{ "1.txt", "1" },
        GetFileExtensionParam{ "111", "111" },
        GetFileExtensionParam{ ".config", ".config" },
        GetFileExtensionParam{ "123.tar.gz", "123.tar" },
        GetFileExtensionParam{ "abc.", "abc" }
    ));

    struct SplitFileNameExtensionParam {
        QString fileName;
        QString expectedName;
        QString expectedExtension;
    };

    class SplitByNameAndExtensionTest : public ::testing::TestWithParam<SplitFileNameExtensionParam> {
    };

    TEST_P(SplitByNameAndExtensionTest, SplitByNameAndExtension_Returns_Correct_Results) {
        QString name;
        QString ext;
        SplitByNameAndExtension(GetParam().fileName, name, ext);
        ASSERT_EQ(name, GetParam().expectedName);
        ASSERT_EQ(ext, GetParam().expectedExtension);
    }

    INSTANTIATE_TEST_SUITE_P(SplitByNameAndExtension, SplitByNameAndExtensionTest, ::testing::Values(
        SplitFileNameExtensionParam{ "1.txt", "1", "txt"},
        SplitFileNameExtensionParam{ "111", "111", ""},
        SplitFileNameExtensionParam{ ".config", ".config", ""},
        SplitFileNameExtensionParam{ "123.tar.gz", "123.tar", "gz"},
        SplitFileNameExtensionParam{ "abc.", "abc", ""}
    ));
}
