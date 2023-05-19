#include <gtest/gtest.h>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include "../fileconflictresolutionpolicy.h"
#include "../diskaccessexception.h"

namespace pboman3::io::test {
    TEST(FileConflictResolutionPolicyTest, ResolvePotentialConflicts_Creates_New_File_Without_Conflicts) {
        const QTemporaryDir temp;
        const FileConflictResolutionPolicy fp(FileConflictResolutionMode::Enum::Abort);
        const auto file = fp.resolvePotentialConflicts(temp.filePath("1.txt"));

        ASSERT_TRUE(file.endsWith("1.txt"));
    }

    TEST(FileConflictResolutionPolicyTest, ResolvePotentialConflicts_Aborts_In_The_Case_Of_Conflict) {
        QTemporaryFile temp;
        temp.open();
        temp.close();

        const FileConflictResolutionPolicy fp(FileConflictResolutionMode::Enum::Abort);

        ASSERT_THROW(fp.resolvePotentialConflicts(temp.fileName()), DiskAccessException);
    }

    TEST(FileConflictResolutionPolicyTest, ResolvePotentialConflicts_Copies_In_The_Case_Of_Conflict) {
        QTemporaryFile temp;
        temp.open();
        temp.close();

        const FileConflictResolutionPolicy fp(FileConflictResolutionMode::Enum::Copy);
        const auto file = fp.resolvePotentialConflicts(temp.fileName());

        ASSERT_NE(file, temp.fileName());
    }

    TEST(FileConflictResolutionPolicyTest, ResolvePotentialConflicts_Overwrites_In_The_Case_Of_Conflict) {
        QTemporaryFile temp;
        temp.open();
        temp.close();

        const FileConflictResolutionPolicy fp(FileConflictResolutionMode::Enum::Overwrite);
        const auto file = fp.resolvePotentialConflicts(temp.fileName());

        ASSERT_EQ(file, temp.fileName());
    }
}
