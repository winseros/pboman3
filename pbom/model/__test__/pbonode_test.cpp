#include "model/pbonode.h"
#include <gtest/gtest.h>
#include "gmock/gmock.h"
#include "model/pboentry.h"
#include "model/pbotreeexception.h"

namespace pboman3::test {
    TEST(PboNodeTest, Ctor_Initializes_Node) {
        PboNode nodeA("a-node", PboNodeType::Folder, nullptr, nullptr);
        ASSERT_EQ(nodeA.nodeType(), PboNodeType::Folder);
        ASSERT_EQ(nodeA.title(), "a-node");
        ASSERT_FALSE(nodeA.par());
        ASSERT_EQ(nodeA.root().get(), &nodeA);

        const PboNode nodeB("b-node", PboNodeType::File, QPointer<PboNode>(&nodeA), nullptr);
        ASSERT_EQ(nodeB.nodeType(), PboNodeType::File);
        ASSERT_EQ(nodeB.title(), "b-node");
        ASSERT_EQ(nodeB.par(), &nodeA);
        ASSERT_EQ(nodeB.root().get(), &nodeB);
    }

    TEST(PboNodeTest, AddEntry_Creates_Tree) {
        const PboEntry e1("e1", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
        const PboEntry e2("f2/e2", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
        const PboEntry e3("f2/e3", PboPackingMethod::Uncompressed, 0, 0, 0, 0);

        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);

        root.addEntry(&e1);
        ASSERT_EQ(root.childCount(), 1);
        ASSERT_EQ(root.child(0)->nodeType(), PboNodeType::File);
        ASSERT_EQ(root.child(0)->title(), "e1");
        ASSERT_EQ(root.child(0)->root(), &root);
        ASSERT_EQ(root.child(0)->par(), &root);

        root.addEntry(&e2);
        ASSERT_EQ(root.childCount(), 2);
        ASSERT_EQ(root.child(1)->nodeType(), PboNodeType::Folder);
        ASSERT_EQ(root.child(1)->title(), "f2");
        ASSERT_EQ(root.child(1)->root(), &root);
        ASSERT_EQ(root.child(1)->par(), &root);

        ASSERT_EQ(root.child(1)->childCount(), 1);
        ASSERT_EQ(root.child(1)->child(0)->nodeType(), PboNodeType::File);
        ASSERT_EQ(root.child(1)->child(0)->title(), "e2");
        ASSERT_EQ(root.child(1)->child(0)->root(), &root);
        ASSERT_EQ(root.child(1)->child(0)->par(), root.child(1));

        root.addEntry(&e3);
        ASSERT_EQ(root.childCount(), 2);

        ASSERT_EQ(root.child(1)->childCount(), 2);
        ASSERT_EQ(root.child(1)->child(1)->nodeType(), PboNodeType::File);
        ASSERT_EQ(root.child(1)->child(1)->title(), "e3");
        ASSERT_EQ(root.child(1)->child(1)->root(), &root);
        ASSERT_EQ(root.child(1)->child(1)->par(), root.child(1));
    }

    TEST(PboNodeTest, MakePath_Returns_Path) {
        const PboEntry e1("e1", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
        const PboEntry e2("f2/e2", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
        const PboEntry e3("f2/e3", PboPackingMethod::Uncompressed, 0, 0, 0, 0);

        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(&e1);
        root.addEntry(&e2);
        root.addEntry(&e3);

        ASSERT_EQ(root.makePath().length(), 0);
        ASSERT_THAT(root.child(0)->makePath(), testing::ElementsAre("e1"));
        ASSERT_THAT(root.child(1)->makePath(), testing::ElementsAre("f2"));
        ASSERT_THAT(root.child(1)->child(0)->makePath(), testing::ElementsAre("f2", "e2"));
        ASSERT_THAT(root.child(1)->child(1)->makePath(), testing::ElementsAre("f2", "e3"));
    }

    TEST(PboNodeTest, Get_Returns_Node) {
        const PboEntry e1("e1", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
        const PboEntry e2("f2/e2", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
        const PboEntry e3("f2/e3", PboPackingMethod::Uncompressed, 0, 0, 0, 0);

        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(&e1);
        root.addEntry(&e2);
        root.addEntry(&e3);

        ASSERT_EQ(root.get(PboPath{ {"e1"}}), root.child(0));
        ASSERT_EQ(root.get(PboPath{ {"f2"}}), root.child(1));
        ASSERT_EQ(root.get(PboPath{ {"f2", "e2"}}), root.child(1)->child(0));
        ASSERT_EQ(root.get(PboPath{ {"f2", "e3"}}), root.child(1)->child(1));
        ASSERT_FALSE(root.get(PboPath{ {"not-existing"}}));
    }

    TEST(PboNodeTest, MoveNode_Moves_Node) {
        const PboEntry e1("e1", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
        const PboEntry e2("f2/e2", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
        const PboEntry e3("f3/e3", PboPackingMethod::Uncompressed, 0, 0, 0, 0);

        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(&e1);
        root.addEntry(&e2);
        root.addEntry(&e3);

        root.moveNode(PboPath{{"e1"}}, PboPath{{"f2"}}, nullptr);
        ASSERT_EQ(root.childCount(), 2);
        ASSERT_EQ(root.child(0)->title(), "f2");
        ASSERT_EQ(root.child(1)->title(), "f3");
        ASSERT_EQ(root.child(0)->childCount(), 2);
        ASSERT_EQ(root.child(0)->child(0)->title(), "e2");
        ASSERT_EQ(root.child(0)->child(1)->title(), "e1");

        root.moveNode(PboPath{{"f3", "e3"}}, PboPath(), nullptr);
        ASSERT_EQ(root.childCount(), 2);
        ASSERT_EQ(root.child(0)->title(), "f2");
        ASSERT_EQ(root.child(1)->title(), "e3");
    }

    TEST(PboNodeTest, MoveNode_Not_Conflicts_Between_Folder_And_File) {
        const PboEntry e1("e1", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
        const PboEntry e2("f2/e1/e2", PboPackingMethod::Uncompressed, 0, 0, 0, 0);

        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(&e1);
        root.addEntry(&e2);

        root.moveNode(PboPath{{"e1"}}, PboPath{{"f2"}}, nullptr);
        ASSERT_EQ(root.childCount(), 1);
        ASSERT_EQ(root.child(0)->title(), "f2");
        ASSERT_EQ(root.child(0)->childCount(), 2);
        ASSERT_EQ(root.child(0)->child(0)->title(), "e1");
        ASSERT_EQ(root.child(0)->child(0)->nodeType(), PboNodeType::Folder);
        ASSERT_EQ(root.child(0)->child(1)->title(), "e1");
        ASSERT_EQ(root.child(0)->child(1)->nodeType(), PboNodeType::File);
    }

    TEST(PboNodeTest, MoveNode_Throws_If_Parent_Is_File) {
        const PboEntry e1("e1", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
        const PboEntry e2("e2", PboPackingMethod::Uncompressed, 0, 0, 0, 0);

        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(&e1);
        root.addEntry(&e2);

        ASSERT_THROW(root.moveNode(PboPath{ {"e1"} }, PboPath{ {"e2"} }, nullptr), PboTreeException);
    }

    TEST(PboNodeTest, MoveNode_Aborts_In_Case_Of_Conflict) {
        const PboEntry e1("e1", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
        const PboEntry e2("f2/e1", PboPackingMethod::Uncompressed, 0, 0, 0, 0);

        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(&e1);
        root.addEntry(&e2);

        const auto onConflict = [](const PboPath& node, PboNodeType nt) { return PboConflictResolution::Abort; };
        root.moveNode(PboPath{{"e1"}}, PboPath{{"f2"}}, onConflict);

        ASSERT_EQ(root.childCount(), 2);
        ASSERT_EQ(root.child(0)->title(), "e1");
        ASSERT_EQ(root.child(1)->title(), "f2");
    }

    TEST(PboNodeTest, MoveNode_Renames_In_Case_Of_Conflict) {
        const PboEntry e1("e1.txt", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
        const PboEntry e2("f2/e1.txt", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
        const PboEntry e3("f2/e1-copy.txt", PboPackingMethod::Uncompressed, 0, 0, 0, 0);

        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(&e1);
        root.addEntry(&e2);
        root.addEntry(&e3);

        const auto onConflict = [](const PboPath& node, PboNodeType nt) { return PboConflictResolution::Copy; };
        root.moveNode(PboPath{{"e1.txt"}}, PboPath{{"f2"}}, onConflict);

        ASSERT_EQ(root.childCount(), 1);
        ASSERT_EQ(root.child(0)->title(), "f2");
        ASSERT_EQ(root.child(0)->childCount(), 3);
        ASSERT_EQ(root.child(0)->child(0)->title(), "e1.txt");
        ASSERT_EQ(root.child(0)->child(1)->title(), "e1-copy.txt");
        ASSERT_EQ(root.child(0)->child(2)->title(), "e1-copy1.txt");
    }

    TEST(PboNodeTest, MoveNode_Replaces_File_In_Case_Of_Conflict) {
        const PboEntry e1("e1.txt", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
        const PboEntry e2("f2/e1.txt", PboPackingMethod::Uncompressed, 0, 0, 0, 0);

        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(&e1);
        root.addEntry(&e2);

        const auto onConflict = [](const PboPath& node, PboNodeType np) {
            return PboConflictResolution::ReplaceOrMerge;
        };
        root.moveNode(PboPath{{"e1.txt"}}, PboPath{{"f2"}}, onConflict);

        ASSERT_EQ(root.childCount(), 1);
        ASSERT_EQ(root.child(0)->title(), "f2");
        ASSERT_EQ(root.child(0)->childCount(), 1);
        ASSERT_EQ(root.child(0)->child(0)->title(), "e1.txt");
    }

    TEST(PboNodeTest, MoveNode_Merges_Folder_In_Case_Of_Conflict) {
        const PboEntry e1("f1/e1.txt", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
        const PboEntry e2("f1/e2.txt", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
        const PboEntry e3("f1/e3.txt", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
        const PboEntry e4("f2/f1/e1.txt", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
        const PboEntry e5("f2/f1/e2.txt", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
        const PboEntry e6("f2/f1/e4.txt", PboPackingMethod::Uncompressed, 0, 0, 0, 0);

        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(&e1);
        root.addEntry(&e2);
        root.addEntry(&e3);
        root.addEntry(&e4);
        root.addEntry(&e5);
        root.addEntry(&e6);

        const auto onConflict = [](const PboPath& node, PboNodeType np) {
            return np == PboNodeType::File ? PboConflictResolution::Copy : PboConflictResolution::ReplaceOrMerge;
        };
        root.moveNode(PboPath{{"f1"}}, PboPath{{"f2"}}, onConflict);

        ASSERT_EQ(root.childCount(), 1);
        ASSERT_EQ(root.child(0)->title(), "f2");
        ASSERT_EQ(root.child(0)->childCount(), 1);
        ASSERT_EQ(root.child(0)->child(0)->title(), "f1");
        ASSERT_EQ(root.child(0)->child(0)->childCount(), 6);
        ASSERT_EQ(root.child(0)->child(0)->child(0)->title(), "e1.txt");
        ASSERT_EQ(root.child(0)->child(0)->child(1)->title(), "e2.txt");
        ASSERT_EQ(root.child(0)->child(0)->child(2)->title(), "e4.txt");
        ASSERT_EQ(root.child(0)->child(0)->child(3)->title(), "e1-copy.txt");
        ASSERT_EQ(root.child(0)->child(0)->child(4)->title(), "e2-copy.txt");
        ASSERT_EQ(root.child(0)->child(0)->child(5)->title(), "e3.txt");
    }

    TEST(PboNodeTest, RenameNode_Renames) {
        const PboEntry e1("e1", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
        const PboEntry e2("e2", PboPackingMethod::Uncompressed, 0, 0, 0, 0);

        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(&e1);
        root.addEntry(&e2);

        root.renameNode(PboPath{ {"e1"} }, "e3");

        ASSERT_EQ(root.childCount(), 2);
        ASSERT_EQ(root.child(0)->title(), "e3");
        ASSERT_EQ(root.child(1)->title(), "e2");
    }

    TEST(PboNodeTest, RenameNode_Does_Not_Rename_If_Conflict) {
        const PboEntry e1("e1", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
        const PboEntry e2("e2", PboPackingMethod::Uncompressed, 0, 0, 0, 0);

        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(&e1);
        root.addEntry(&e2);

        root.renameNode(PboPath{ {"e1"} }, "e2");

        ASSERT_EQ(root.childCount(), 2);
        ASSERT_EQ(root.child(0)->title(), "e1");
        ASSERT_EQ(root.child(1)->title(), "e2");
    }

    TEST(PboNodeTest, RemoveNode_Removes) {
        const PboEntry e1("e1", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
        const PboEntry e2("f2/e2", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
        const PboEntry e3("f2/e3", PboPackingMethod::Uncompressed, 0, 0, 0, 0);

        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(&e1);
        root.addEntry(&e2);
        root.addEntry(&e3);

        root.removeNode(PboPath{ {"e1"} });
        ASSERT_EQ(root.childCount(), 1);
        ASSERT_EQ(root.child(0)->title(), "f2");
        ASSERT_EQ(root.child(0)->childCount(), 2);

        root.removeNode(PboPath{ {"f2", "e2"} });
        ASSERT_EQ(root.childCount(), 1);
        ASSERT_EQ(root.child(0)->title(), "f2");
        ASSERT_EQ(root.child(0)->childCount(), 1);
        ASSERT_EQ(root.child(0)->child(0)->title(), "e3");

        root.removeNode(PboPath{ {"f2", "e3"} });
        ASSERT_EQ(root.childCount(), 0);
    }
}
