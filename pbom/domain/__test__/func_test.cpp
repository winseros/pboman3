#include <gtest/gtest.h>
#include "domain/func.h"
#include "domain/pbonode.h"

namespace pboman3::domain::test {
    TEST(FuncTest, CountFilesInTree_Counts_Files) {
        const auto node = QSharedPointer<PboNode>::create("file.pbo", PboNodeType::Container, nullptr);
        node->createHierarchy(PboPath("f1/e1.txt"));
        node->createHierarchy(PboPath("f1/e2.txt"));
        node->createHierarchy(PboPath("f1/f2/e2.txt"));
        node->createHierarchy(PboPath("e2.txt"));

        int count = 0;
        CountFilesInTree(*node, count);

        ASSERT_EQ(count, 4);
    }

    TEST(FuncTest, IsPathConflict_Functional) {
        const auto root = QSharedPointer<PboNode>::create("file-name", PboNodeType::Container, nullptr);
        ASSERT_EQ(root->depth(), 0);

        root->createHierarchy(PboPath("e1.txt"));
        root->createHierarchy(PboPath("f2/e2.txt"));

        ASSERT_TRUE(IsPathConflict(root.get(), PboPath("e1.txT")));
        ASSERT_TRUE(IsPathConflict(root.get(), PboPath("f2")));
        ASSERT_TRUE(IsPathConflict(root.get(), PboPath("F2/e2.Txt")));
        ASSERT_TRUE(IsPathConflict(root.get(), PboPath("f2/E2.txt/e4.txt")));

        ASSERT_FALSE(IsPathConflict(root.get(), PboPath("e2.txt")));
        ASSERT_FALSE(IsPathConflict(root.get(), PboPath("f2/e3.txt")));
        ASSERT_FALSE(IsPathConflict(root.get(), PboPath("f3/e4.txt")));
    }
}
