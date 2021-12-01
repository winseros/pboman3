#include <gtest/gtest.h>
#include "domain/func.h"
#include "domain/pbonode.h"

namespace pboman3::domain::test {
    TEST(FuncTest, CountFilesInTree_Counts_Files) {
        PboNode node("file.pbo", PboNodeType::Container, nullptr);
        node.createHierarchy(PboPath("f1/e1.txt"));
        node.createHierarchy(PboPath("f1/e2.txt"));
        node.createHierarchy(PboPath("f1/f2/e2.txt"));
        node.createHierarchy(PboPath("e2.txt"));

        int count = 0;
        CountFilesInTree(node, count);

        ASSERT_EQ(count, 4);
    }

    TEST(FuncTest, FindDirectChild_Finds_Child_Case_Insensitively_Const) {
        PboNode node("file.pbo", PboNodeType::Container, nullptr);
        node.createHierarchy(PboPath("e1.txt"));
        node.createHierarchy(PboPath("e2.txt"));
        PboNode* e3 = node.createHierarchy(PboPath("e3.txt"));

        const PboNode* found = FindDirectChild(const_cast<const PboNode*>(&node), "E3.TxT");

        ASSERT_EQ(e3, found);
    }

    TEST(FuncTest, FindDirectChild_Returns_Null_Const) {
        PboNode node("file.pbo", PboNodeType::Container, nullptr);
        node.createHierarchy(PboPath("e1.txt"));
        node.createHierarchy(PboPath("e2.txt"));
        node.createHierarchy(PboPath("e3.txt"));

        const PboNode* found = FindDirectChild(const_cast<const PboNode*>(&node), "e4.txt");

        ASSERT_EQ(found, nullptr);
    }

    TEST(FuncTest, FindDirectChild_Finds_Child_Case_Insensitively_NonConst) {
        PboNode node("file.pbo", PboNodeType::Container, nullptr);
        node.createHierarchy(PboPath("e1.txt"));
        node.createHierarchy(PboPath("e2.txt"));
        PboNode* e3 = node.createHierarchy(PboPath("e3.txt"));

        const PboNode* found = FindDirectChild(&node, "E3.TxT");

        ASSERT_EQ(e3, found);
    }

    TEST(FuncTest, FindDirectChild_Returns_Null_NonConst) {
        PboNode node("file.pbo", PboNodeType::Container, nullptr);
        node.createHierarchy(PboPath("e1.txt"));
        node.createHierarchy(PboPath("e2.txt"));
        node.createHierarchy(PboPath("e3.txt"));

        const PboNode* found = FindDirectChild(&node, "e4.txt");

        ASSERT_EQ(found, nullptr);
    }

    TEST(FuncTest, IsPathConflict_Functional) {
        PboNode root("file-name", PboNodeType::Container, nullptr);
        ASSERT_EQ(root.depth(), 0);

        root.createHierarchy(PboPath("e1.txt"));
        root.createHierarchy(PboPath("f2/e2.txt"));

        ASSERT_TRUE(IsPathConflict(&root, PboPath("e1.txT")));
        ASSERT_TRUE(IsPathConflict(&root, PboPath("f2")));
        ASSERT_TRUE(IsPathConflict(&root, PboPath("F2/e2.Txt")));
        ASSERT_TRUE(IsPathConflict(&root, PboPath("f2/E2.txt/e4.txt")));

        ASSERT_FALSE(IsPathConflict(&root, PboPath("e2.txt")));
        ASSERT_FALSE(IsPathConflict(&root, PboPath("f2/e3.txt")));
        ASSERT_FALSE(IsPathConflict(&root, PboPath("f3/e4.txt")));
    }
}
