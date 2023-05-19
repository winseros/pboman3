#include <gtest/gtest.h>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include "../filefactory.h"
#include "../diskaccessexception.h"

namespace pboman3::io::test {
    TEST(FileFactoryTest, CreateFile_Creates_New_File_Without_Conflicts) {
        const QTemporaryDir temp;
        const FileFactory ff(FileConflictResolutionMode::Enum::Abort);
        const auto file = ff.createFile(temp.filePath("1.txt"));

        ASSERT_TRUE(file->fileName().endsWith("1.txt"));
    }

    TEST(FileFactoryTest, CreateFile_Aborts_In_The_Case_Of_Conflict) {
        QTemporaryFile temp;
        temp.open();
        temp.close();

        const FileFactory ff(FileConflictResolutionMode::Enum::Abort);

        ASSERT_THROW(ff.createFile(temp.fileName()), DiskAccessException);
    }

    TEST(FileFactoryTest, CreateFile_Copies_In_The_Case_Of_Conflict) {
        QTemporaryFile temp;
        temp.open();
        temp.close();

        const FileFactory ff(FileConflictResolutionMode::Enum::Copy);
        const auto file = ff.createFile(temp.fileName());

        ASSERT_NE(file->fileName(), temp.fileName());
    }

    TEST(FileFactoryTest, CreateFile_Overwrites_In_The_Case_Of_Conflict) {
        QTemporaryFile temp;
        temp.open();
        temp.close();

        const FileFactory ff(FileConflictResolutionMode::Enum::Overwrite);
        const auto file = ff.createFile(temp.fileName());

        ASSERT_EQ(file->fileName(), temp.fileName());
    }
}
