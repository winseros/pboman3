#include "model/treeconflicts.h"

#include <gtest/gtest.h>
#include "model/pbonode.h"
#include "model/pbopath.h"

namespace pboman3::test {
    TEST(TreeConflictsTest, Inspect_Inflates_Conflicting_Entries) {
        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(PboPath("e1"));
        root.addEntry(PboPath("f2/e2"));

        TreeConflicts conflicts;
        conflicts.inspect(&root, "e1");
        conflicts.inspect(&root, "f2");
        conflicts.inspect(&root, "f3");

        ASSERT_EQ(conflicts.count(), 1);
        ASSERT_EQ(*conflicts.keyBegin(), "e1");
    }

    TEST(TreeConflictsTest, GetResolution_Returns_Correct_Results) {
        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(PboPath("e1"));

        TreeConflicts conflicts;
        conflicts.inspect(&root, "e1");

        ASSERT_EQ(conflicts.getResolution("e1"), TreeConflictResolution::Copy);
        ASSERT_EQ(conflicts.getResolution("e2"), TreeConflictResolution::Throw);
    }
}
