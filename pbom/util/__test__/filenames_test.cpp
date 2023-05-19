#include <QString>
#include <QTemporaryDir>
#include <gtest/gtest.h>
#include "util/filenames.h"

namespace pboman3::util::test {
    TEST(FileNamesTest, GetCopyFolderName_Returns_Valid_Copy_Name) {
        const QTemporaryDir tempDir;

        const auto copyPath = FileNames::getCopyFolderName(tempDir.path(), 1);

        const auto expectedPath = tempDir.path() + "(1)";

        ASSERT_EQ(copyPath, expectedPath);
    }

    struct FileNameParam {
        QString fileName;
        QString expectedResult;
    };

    class GetCopyFileNameTest : public ::testing::TestWithParam<FileNameParam> {
    };

    TEST_P(GetCopyFileNameTest, GetCopyFileName_Returns_Valid_Copy_Name) {
        const QTemporaryDir temporaryDir;

        const auto filePath = temporaryDir.filePath(GetParam().fileName);
        QFile file(filePath);
        file.open(QIODeviceBase::Append);
        file.close();

        const auto copyPath = FileNames::getCopyFileName(filePath, 1);

        const QFileInfo fi(copyPath);
        ASSERT_EQ(fi.fileName(), GetParam().expectedResult);
    }

    INSTANTIATE_TEST_SUITE_P(FileNamesTest, GetCopyFileNameTest, ::testing::Values(
                                 FileNameParam{"f.txt", "f(1).txt"},
                                 FileNameParam{"f", "f(1)"},
                                 FileNameParam{".txt", "(1).txt"}
                             ));

    class GetFileExtensionTest : public ::testing::TestWithParam<FileNameParam> {
    };

    TEST_P(GetFileExtensionTest, GetFileExtension_Returns_Correct_Results) {
        const QString result = FileNames::getFileExtension(GetParam().fileName);
        ASSERT_EQ(result, GetParam().expectedResult);
    }

    INSTANTIATE_TEST_SUITE_P(FileNamesTest, GetFileExtensionTest, ::testing::Values(
                                 FileNameParam{"1.txt", "txt"},
                                 FileNameParam{"111", ""},
                                 FileNameParam{".config", "config"},
                                 FileNameParam{"123.tar.gz", "gz"},
                                 FileNameParam{"abc.", ""}
                             ));

    class GetFileNameWithoutExtensionTest : public ::testing::TestWithParam<FileNameParam> {
    };

    TEST_P(GetFileNameWithoutExtensionTest, GetFileNameWithoutExtension_Returns_Correct_Results) {
        const QString result = FileNames::getFileNameWithoutExtension(GetParam().fileName);
        ASSERT_EQ(result, GetParam().expectedResult);
    }

    INSTANTIATE_TEST_SUITE_P(FileNamesTest, GetFileNameWithoutExtensionTest, ::testing::Values(
                                 FileNameParam{ "1.txt", "1" },
                                 FileNameParam{ "111", "111" },
                                 FileNameParam{ ".config", ".config" },
                                 FileNameParam{ "123.tar.gz", "123.tar" },
                                 FileNameParam{ "abc.", "abc" }
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
        FileNames::splitByNameAndExtension(GetParam().fileName, name, ext);
        ASSERT_EQ(name, GetParam().expectedName);
        ASSERT_EQ(ext, GetParam().expectedExtension);
    }

    INSTANTIATE_TEST_SUITE_P(FileNamesTest, SplitByNameAndExtensionTest, ::testing::Values(
                                 SplitFileNameExtensionParam{ "1.txt", "1", "txt"},
                                 SplitFileNameExtensionParam{ "111", "111", ""},
                                 SplitFileNameExtensionParam{ ".config", ".config", ""},
                                 SplitFileNameExtensionParam{ "123.tar.gz", "123.tar", "gz"},
                                 SplitFileNameExtensionParam{ "abc.", "abc", ""}
                             ));
}
