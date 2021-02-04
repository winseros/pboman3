#include "treenode.h"
#include "gtest/gtest.h"

namespace pboman3::test {
    using namespace pboman3;

    TEST(TreeNodeTest, CtorInitializesObject1) {
        PboEntry_ entry("some-file-name", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
        const TreeNode node("some-title", TreeNodeType::Dir, nullptr, &entry);

        ASSERT_EQ(node.title(), "some-title");
        ASSERT_EQ(node.parent(), nullptr);
        ASSERT_EQ(node.entry, &entry);
        ASSERT_EQ(node.row(), 1);
    }

    TEST(TreeNodeTest, CtorInitializesObject2) {
        const RootNode root("root-name");
        const TreeNode node("some-container", &root);

        ASSERT_EQ(node.title(), "some-container");
        ASSERT_EQ(node.parent(), &root);
        ASSERT_EQ(root.childCount(), 1);
    }

    TEST(TreeNodeTest, AddEntryAddsNewEntries) {
        const PboEntry_ e1{"e1", PboPackingMethod::Uncompressed, 0, 0, 0, 0};
        const PboEntry_ e2{"f2/e2", PboPackingMethod::Uncompressed, 0, 0, 0, 0};
        const PboEntry_ e3{"f2/e3", PboPackingMethod::Uncompressed, 0, 0, 0, 0};
        const PboEntry_ e4{"f1/f3/e4", PboPackingMethod::Uncompressed, 0, 0, 0, 0};
        const PboEntry_ e5{"f1/f3/e5", PboPackingMethod::Uncompressed, 0, 0, 0, 0};
        const PboEntry_ e6{"f2/e6", PboPackingMethod::Uncompressed, 0, 0, 0, 0};

        //add entries
        RootNode root("root-name");
        root.addEntry(&e1);
        root.addEntry(&e2);
        root.addEntry(&e3);
        root.addEntry(&e4);
        root.addEntry(&e5);
        root.addEntry(&e6);

        //root
        ASSERT_EQ(root.childCount(), 1);
        ASSERT_EQ(root.child(0)->childCount(), 3);

        //f1
        ASSERT_EQ(root.child(0)->child(0)->title(), "f1");
        ASSERT_EQ(root.child(0)->child(0)->path(), "f1");
        ASSERT_EQ(root.child(0)->child(0)->parent(), root.child(0));
        ASSERT_EQ(root.child(0)->child(0)->childCount(), 1);
        ASSERT_FALSE(root.child(0)->child(0)->entry);

        //f1/f3
        ASSERT_EQ(root.child(0)->child(0)->child(0)->title(), "f3");
        ASSERT_EQ(root.child(0)->child(0)->child(0)->path(), "f1\\f3");
        ASSERT_EQ(root.child(0)->child(0)->child(0)->parent(), root.child(0)->child(0));
        ASSERT_EQ(root.child(0)->child(0)->child(0)->childCount(), 2);
        ASSERT_FALSE(root.child(0)->child(0)->child(0)->entry);

        //f1/f3/e4
        ASSERT_EQ(root.child(0)->child(0)->child(0)->child(0)->title(), "e4");
        ASSERT_EQ(root.child(0)->child(0)->child(0)->child(0)->path(), "f1\\f3\\e4");
        ASSERT_EQ(root.child(0)->child(0)->child(0)->child(0)->parent(), root.child(0)->child(0)->child(0));
        ASSERT_EQ(root.child(0)->child(0)->child(0)->child(0)->childCount(), 0);
        ASSERT_EQ(root.child(0)->child(0)->child(0)->child(0)->entry, &e4);

        //f1/f3/e5
        ASSERT_EQ(root.child(0)->child(0)->child(0)->child(1)->title(), "e5");
        ASSERT_EQ(root.child(0)->child(0)->child(0)->child(1)->path(), "f1\\f3\\e5");
        ASSERT_EQ(root.child(0)->child(0)->child(0)->child(1)->parent(), root.child(0)->child(0)->child(0));
        ASSERT_EQ(root.child(0)->child(0)->child(0)->child(1)->childCount(), 0);
        ASSERT_EQ(root.child(0)->child(0)->child(0)->child(1)->entry, &e5);

        //f2
        ASSERT_EQ(root.child(0)->child(1)->title(), "f2");
        ASSERT_EQ(root.child(0)->child(1)->path(), "f2");
        ASSERT_EQ(root.child(0)->child(1)->parent(), root.child(0));
        ASSERT_EQ(root.child(0)->child(1)->childCount(), 3);
        ASSERT_FALSE(root.child(0)->child(1)->entry);

        //f2/e2
        ASSERT_EQ(root.child(0)->child(1)->child(0)->title(), "e2");
        ASSERT_EQ(root.child(0)->child(1)->child(0)->path(), "f2\\e2");
        ASSERT_EQ(root.child(0)->child(1)->child(0)->parent(), root.child(0)->child(1));
        ASSERT_EQ(root.child(0)->child(1)->child(0)->childCount(), 0);
        ASSERT_EQ(root.child(0)->child(1)->child(0)->entry, &e2);

        //f2/e3
        ASSERT_EQ(root.child(0)->child(1)->child(1)->title(), "e3");
        ASSERT_EQ(root.child(0)->child(1)->child(1)->path(), "f2\\e3");
        ASSERT_EQ(root.child(0)->child(1)->child(1)->parent(), root.child(0)->child(1));
        ASSERT_EQ(root.child(0)->child(1)->child(1)->childCount(), 0);
        ASSERT_EQ(root.child(0)->child(1)->child(1)->entry, &e3);

        //f2/e6
        ASSERT_EQ(root.child(0)->child(1)->child(2)->title(), "e6");
        ASSERT_EQ(root.child(0)->child(1)->child(2)->path(), "f2\\e6");
        ASSERT_EQ(root.child(0)->child(1)->child(2)->parent(), root.child(0)->child(1));
        ASSERT_EQ(root.child(0)->child(1)->child(2)->childCount(), 0);
        ASSERT_EQ(root.child(0)->child(1)->child(2)->entry, &e6);
    }
}
