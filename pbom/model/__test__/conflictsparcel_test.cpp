#include <gtest/gtest.h>
#include "model/conflictsparcel.h"

namespace pboman3::test {
    TEST(ConflictsParcelTest, GetResolution_Returns_Unset_If_Resolution_Was_Not_Set) {
        const ConflictsParcel conflicts;
        const ConflictResolution res = conflicts.getResolution(NodeDescriptor(nullptr, "some-path"));
        ASSERT_EQ(res, ConflictResolution::Unset);
    }

    TEST(ConflictsParcelTest, GetResolution_Returns_Resolution_If_Resolution_Was_Set) {
        const NodeDescriptor nd(nullptr, "some-path");
        ConflictsParcel conflicts;
        conflicts.setResolution(nd, ConflictResolution::Copy);
        const ConflictResolution res = conflicts.getResolution(nd);
        ASSERT_EQ(res, ConflictResolution::Copy);
    }

    TEST(ConflictsParcelTest, HasConflicts_Returns_Correct_Values) {
        ConflictsParcel conflicts;
        ASSERT_FALSE(conflicts.hasConflicts());

        conflicts.setResolution(NodeDescriptor(nullptr, "some-path"), ConflictResolution::Copy);
        ASSERT_TRUE(conflicts.hasConflicts());
    }
}
