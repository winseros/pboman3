#include "treenode.h"
#include <QStandardItemModel>
#include "gtest/gtest.h"

namespace pboman3::test {
    using namespace pboman3;

    TEST(TreeNodeTest, CtorInitializesObject1) {
        PboEntry_ entry("some-file-name", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
        const TreeNode node("some-title", TreeNodeType::Dir, nullptr, &entry);

        ASSERT_EQ(node.title(), "some-title");
        ASSERT_EQ(node.parentNode(), nullptr);
        ASSERT_EQ(node.entry(), &entry);
        ASSERT_EQ(node.row(), 1);
    }

    TEST(TreeNodeTest, CtorInitializesObject2) {
        RootNode root("root-name");
        const TreeNode node("some-container", QPointer<TreeNode>(&root));

        ASSERT_EQ(node.title(), "some-container");
        ASSERT_EQ(node.parentNode(), &root);
        ASSERT_EQ(root.childNodeCount(), 1);
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
        ASSERT_EQ(root.childNodeCount(), 1);
        ASSERT_EQ(root.childNode(0)->childNodeCount(), 3);

        //f1
        ASSERT_EQ(root.childNode(0)->childNode(0)->title(), "f1");
        ASSERT_EQ(root.childNode(0)->childNode(0)->path(), "f1");
        ASSERT_EQ(root.childNode(0)->childNode(0)->parentNode(), root.childNode(0));
        ASSERT_EQ(root.childNode(0)->childNode(0)->childNodeCount(), 1);
        ASSERT_FALSE(root.childNode(0)->childNode(0)->entry());

        //f1/f3
        ASSERT_EQ(root.childNode(0)->childNode(0)->childNode(0)->title(), "f3");
        ASSERT_EQ(root.childNode(0)->childNode(0)->childNode(0)->path(), "f1\\f3");
        ASSERT_EQ(root.childNode(0)->childNode(0)->childNode(0)->parentNode(), root.childNode(0)->childNode(0));
        ASSERT_EQ(root.childNode(0)->childNode(0)->childNode(0)->childNodeCount(), 2);
        ASSERT_FALSE(root.childNode(0)->childNode(0)->childNode(0)->entry());

        //f1/f3/e4
        ASSERT_EQ(root.childNode(0)->childNode(0)->childNode(0)->childNode(0)->title(), "e4");
        ASSERT_EQ(root.childNode(0)->childNode(0)->childNode(0)->childNode(0)->path(), "f1\\f3\\e4");
        ASSERT_EQ(root.childNode(0)->childNode(0)->childNode(0)->childNode(0)->parentNode(),
                  root.childNode(0)->childNode(0)->childNode(0));
        ASSERT_EQ(root.childNode(0)->childNode(0)->childNode(0)->childNode(0)->childNodeCount(), 0);
        ASSERT_EQ(root.childNode(0)->childNode(0)->childNode(0)->childNode(0)->entry(), &e4);

        //f1/f3/e5
        ASSERT_EQ(root.childNode(0)->childNode(0)->childNode(0)->childNode(1)->title(), "e5");
        ASSERT_EQ(root.childNode(0)->childNode(0)->childNode(0)->childNode(1)->path(), "f1\\f3\\e5");
        ASSERT_EQ(root.childNode(0)->childNode(0)->childNode(0)->childNode(1)->parentNode(),
                  root.childNode(0)->childNode(0)->childNode(0));
        ASSERT_EQ(root.childNode(0)->childNode(0)->childNode(0)->childNode(1)->childNodeCount(), 0);
        ASSERT_EQ(root.childNode(0)->childNode(0)->childNode(0)->childNode(1)->entry(), &e5);

        //f2
        ASSERT_EQ(root.childNode(0)->childNode(1)->title(), "f2");
        ASSERT_EQ(root.childNode(0)->childNode(1)->path(), "f2");
        ASSERT_EQ(root.childNode(0)->childNode(1)->parentNode(), root.childNode(0));
        ASSERT_EQ(root.childNode(0)->childNode(1)->childNodeCount(), 3);
        ASSERT_FALSE(root.childNode(0)->childNode(1)->entry());

        //f2/e2
        ASSERT_EQ(root.childNode(0)->childNode(1)->childNode(0)->title(), "e2");
        ASSERT_EQ(root.childNode(0)->childNode(1)->childNode(0)->path(), "f2\\e2");
        ASSERT_EQ(root.childNode(0)->childNode(1)->childNode(0)->parentNode(), root.childNode(0)->childNode(1));
        ASSERT_EQ(root.childNode(0)->childNode(1)->childNode(0)->childNodeCount(), 0);
        ASSERT_EQ(root.childNode(0)->childNode(1)->childNode(0)->entry(), &e2);

        //f2/e3
        ASSERT_EQ(root.childNode(0)->childNode(1)->childNode(1)->title(), "e3");
        ASSERT_EQ(root.childNode(0)->childNode(1)->childNode(1)->path(), "f2\\e3");
        ASSERT_EQ(root.childNode(0)->childNode(1)->childNode(1)->parentNode(), root.childNode(0)->childNode(1));
        ASSERT_EQ(root.childNode(0)->childNode(1)->childNode(1)->childNodeCount(), 0);
        ASSERT_EQ(root.childNode(0)->childNode(1)->childNode(1)->entry(), &e3);

        //f2/e6
        ASSERT_EQ(root.childNode(0)->childNode(1)->childNode(2)->title(), "e6");
        ASSERT_EQ(root.childNode(0)->childNode(1)->childNode(2)->path(), "f2\\e6");
        ASSERT_EQ(root.childNode(0)->childNode(1)->childNode(2)->parentNode(), root.childNode(0)->childNode(1));
        ASSERT_EQ(root.childNode(0)->childNode(1)->childNode(2)->childNodeCount(), 0);
        ASSERT_EQ(root.childNode(0)->childNode(1)->childNode(2)->entry(), &e6);
    }

    TEST(TreeNodeTest, ScheduleRemove_Schedules) {
        const PboEntry_ e1{"e1", PboPackingMethod::Uncompressed, 0, 0, 0, 0};
        const PboEntry_ e2{"f2/e2", PboPackingMethod::Uncompressed, 0, 0, 0, 0};
        const PboEntry_ e3{"f2/e3", PboPackingMethod::Uncompressed, 0, 0, 0, 0};

        //add entries
        RootNode root("root-name");
        root.addEntry(&e1);
        root.addEntry(&e2);
        root.addEntry(&e3);

        //mark e1
        root.scheduleRemove(&e1);
        ASSERT_FALSE(root.font().strikeOut());
        ASSERT_FALSE(root.childNode(0)->font().strikeOut());
        ASSERT_FALSE(root.childNode(0)->childNode(0)->font().strikeOut()); //f2
        ASSERT_FALSE(root.childNode(0)->childNode(0)->childNode(0)->font().strikeOut()); //f2/e2
        ASSERT_FALSE(root.childNode(0)->childNode(0)->childNode(1)->font().strikeOut()); //f2/e3
        ASSERT_TRUE(root.childNode(0)->childNode(1)->font().strikeOut()); //e1

        //mark e2
        root.scheduleRemove(&e2);
        ASSERT_FALSE(root.font().strikeOut());
        ASSERT_FALSE(root.childNode(0)->font().strikeOut());
        ASSERT_FALSE(root.childNode(0)->childNode(0)->font().strikeOut()); //f2
        ASSERT_TRUE(root.childNode(0)->childNode(0)->childNode(0)->font().strikeOut()); //f2/e2
        ASSERT_FALSE(root.childNode(0)->childNode(0)->childNode(1)->font().strikeOut()); //f2/e3
        ASSERT_TRUE(root.childNode(0)->childNode(1)->font().strikeOut()); //e1

        //mark e3
        root.scheduleRemove(&e3);
        ASSERT_FALSE(root.font().strikeOut());
        ASSERT_FALSE(root.childNode(0)->font().strikeOut());
        ASSERT_TRUE(root.childNode(0)->childNode(0)->font().strikeOut()); //f2
        ASSERT_TRUE(root.childNode(0)->childNode(0)->childNode(0)->font().strikeOut()); //f2/e2
        ASSERT_TRUE(root.childNode(0)->childNode(0)->childNode(1)->font().strikeOut()); //f2/e3
        ASSERT_TRUE(root.childNode(0)->childNode(1)->font().strikeOut()); //e1

        //unmark e3
        root.scheduleRemove(&e3, false);
        ASSERT_FALSE(root.font().strikeOut());
        ASSERT_FALSE(root.childNode(0)->font().strikeOut());
        ASSERT_FALSE(root.childNode(0)->childNode(0)->font().strikeOut()); //f2
        ASSERT_TRUE(root.childNode(0)->childNode(0)->childNode(0)->font().strikeOut()); //f2/e2
        ASSERT_FALSE(root.childNode(0)->childNode(0)->childNode(1)->font().strikeOut()); //f2/e3
        ASSERT_TRUE(root.childNode(0)->childNode(1)->font().strikeOut()); //e1
    }

    TEST(TreeNodeTest, CompleteRemove_Removes) {
        const PboEntry_ e1{"e1", PboPackingMethod::Uncompressed, 0, 0, 0, 0};
        const PboEntry_ e2{"f2/e2", PboPackingMethod::Uncompressed, 0, 0, 0, 0};
        const PboEntry_ e3{"f2/e3", PboPackingMethod::Uncompressed, 0, 0, 0, 0};

        //add entries
        RootNode root("root-name");
        root.addEntry(&e1);
        root.addEntry(&e2);
        root.addEntry(&e3);

        //remove e1
        root.completeRemove(&e1);
        ASSERT_EQ(root.childNode(0)->childNodeCount(), 1);
        ASSERT_EQ(root.childNode(0)->childNode(0)->title(), "f2");
        ASSERT_EQ(root.childNode(0)->childNode(0)->childNodeCount(), 2);

        //remove e2
        root.completeRemove(&e2);
        ASSERT_EQ(root.childNode(0)->childNodeCount(), 1);
        ASSERT_EQ(root.childNode(0)->childNode(0)->title(), "f2");
        ASSERT_EQ(root.childNode(0)->childNode(0)->childNodeCount(), 1);
        ASSERT_EQ(root.childNode(0)->childNode(0)->childNode(0)->title(), "e3");

        //remove e3
        root.completeRemove(&e3);
        ASSERT_EQ(root.childNode(0)->childNodeCount(), 0);
    }

    class MockItemModel : public QStandardItemModel {
    public:
        QModelIndex createModelIndex(int id) const {
            return createIndex(id, 0, nullptr);
        }
    };

    TEST(TreeNodeTest, PrepareRemove_Returns_Deepest_Removed_Node1) {
        const PboEntry_ e1{"e1", PboPackingMethod::Uncompressed, 0, 0, 0, 0};
        const PboEntry_ e2{"f2/e2", PboPackingMethod::Uncompressed, 0, 0, 0, 0};
        const PboEntry_ e3{"f2/e3", PboPackingMethod::Uncompressed, 0, 0, 0, 0};

        //add entries
        RootNode root("root-name");
        root.addEntry(&e1);
        root.addEntry(&e2);
        root.addEntry(&e3);

        //check the method
        const QPointer<TreeNode> eE3 = root.prepareRemove(&e3);
        ASSERT_EQ(eE3.get(), root.childNode(0)->childNode(0)->childNode(1).get());

        const QPointer<TreeNode> eE1 = root.prepareRemove(&e1);
        ASSERT_EQ(eE1.get(), root.childNode(0)->childNode(1).get());
    }

    TEST(TreeNodeTest, PrepareRemove_Returns_Deepest_Removed_Node2) {
        const PboEntry_ e1{"e1", PboPackingMethod::Uncompressed, 0, 0, 0, 0};
        const PboEntry_ e2{"f2/e2", PboPackingMethod::Uncompressed, 0, 0, 0, 0};

        //add entries
        RootNode root("root-name");
        root.addEntry(&e1);
        root.addEntry(&e2);

        //check the method
        const QPointer<TreeNode> eE2 = root.prepareRemove(&e2);
        ASSERT_EQ(eE2.get(), root.childNode(0)->childNode(0).get());

        const QPointer<TreeNode> eE1 = root.prepareRemove(&e1);
        ASSERT_EQ(eE1.get(), root.childNode(0)->childNode(1).get());
    }
}
