#include "model/pbonode.h"
#include <QTemporaryFile>
#include <gtest/gtest.h>
#include "gmock/gmock.h"
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

    TEST(PboNodeTest, AddEntry1_Creates_Tree) {
        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);

        root.addEntry(PboPath("e1"));
        ASSERT_EQ(root.childCount(), 1);
        ASSERT_EQ(root.child(0)->nodeType(), PboNodeType::File);
        ASSERT_EQ(root.child(0)->title(), "e1");
        ASSERT_EQ(root.child(0)->root(), &root);
        ASSERT_EQ(root.child(0)->par(), &root);

        root.addEntry(PboPath("f2/e2"));
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

        root.addEntry(PboPath("f2/e3"));
        ASSERT_EQ(root.childCount(), 2);

        ASSERT_EQ(root.child(1)->childCount(), 2);
        ASSERT_EQ(root.child(1)->child(1)->nodeType(), PboNodeType::File);
        ASSERT_EQ(root.child(1)->child(1)->title(), "e3");
        ASSERT_EQ(root.child(1)->child(1)->root(), &root);
        ASSERT_EQ(root.child(1)->child(1)->par(), root.child(1));
    }

    TEST(PboNodeTest, AddEntry2_Creates_Tree_If_Entries_Make_No_Conflicts) {
        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);

        QPointer<PboNode> e1 = root.addEntry(PboPath("e1"), TreeConflictResolution::Throw);
        ASSERT_EQ(root.childCount(), 1);

        //e1 check
        ASSERT_TRUE(e1);
        ASSERT_EQ(e1->nodeType(), PboNodeType::File);

        ASSERT_EQ(root.child(0)->nodeType(), PboNodeType::File);
        ASSERT_EQ(root.child(0)->title(), "e1");
        ASSERT_EQ(root.child(0)->root(), &root);
        ASSERT_EQ(root.child(0)->par(), &root);

        //f2 check
        QPointer<PboNode> e2 = root.addEntry(PboPath("f2/e2"), TreeConflictResolution::Throw);

        ASSERT_TRUE(e2);
        ASSERT_EQ(e2->nodeType(), PboNodeType::File);

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

        //f3 check
        QPointer<PboNode> e3 = root.addEntry(PboPath("f2/e3"), TreeConflictResolution::Throw);

        ASSERT_TRUE(e3);
        ASSERT_EQ(e3->nodeType(), PboNodeType::File);

        ASSERT_EQ(root.childCount(), 2);
        ASSERT_EQ(root.child(1)->childCount(), 2);
        ASSERT_EQ(root.child(1)->child(1)->nodeType(), PboNodeType::File);
        ASSERT_EQ(root.child(1)->child(1)->title(), "e3");
        ASSERT_EQ(root.child(1)->child(1)->root(), &root);
        ASSERT_EQ(root.child(1)->child(1)->par(), root.child(1));
    }

    TEST(PboNodeTest, AddEntry2_Replaces_Conflicting_Node) {
        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);

        root.addEntry(PboPath("f2/e1"));
        const QPointer<PboNode> e1Old = root.child(0)->child(0);

        const QPointer<PboNode> e1New = root.addEntry(PboPath("f2/e1"), TreeConflictResolution::Replace);

        ASSERT_EQ(root.child(0)->childCount(), 1);
        ASSERT_EQ(root.child(0)->child(0), e1New);
        ASSERT_TRUE(e1Old.isNull());
    }

    TEST(PboNodeTest, AddEntry2_Renames_Conflicting_Node) {
        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);

        root.addEntry(PboPath("e1"));
        root.addEntry(PboPath("e1.txt"));
        root.addEntry(PboPath("f2/e1.txt"));
        root.addEntry(PboPath("f2/e1-copy.txt"));

        const QPointer<PboNode> c1Old = root.child(0);
        const QPointer<PboNode> c2Old = root.child(1);
        const QPointer<PboNode> c3Old = root.child(2)->child(0);
        const QPointer<PboNode> c4Old = root.child(2)->child(1);

        const QPointer<PboNode> c1New = root.addEntry(PboPath("e1"), TreeConflictResolution::Copy);
        const QPointer<PboNode> c2New = root.addEntry(PboPath("e1.txt"), TreeConflictResolution::Copy);
        const QPointer<PboNode> c3New = root.addEntry(PboPath("f2/e1.txt"), TreeConflictResolution::Copy);
        const QPointer<PboNode> c4New = root.addEntry(PboPath("f2/e1-copy.txt"), TreeConflictResolution::Copy);

        ASSERT_EQ(root.childCount(), 5);
        ASSERT_EQ(root.child(0), c1Old);
        ASSERT_EQ(root.child(1), c2Old);

        ASSERT_EQ(root.child(2)->childCount(), 4);
        ASSERT_EQ(root.child(2)->child(0), c3Old);
        ASSERT_EQ(root.child(2)->child(1), c4Old);
        ASSERT_EQ(root.child(2)->child(2), c3New);
        ASSERT_EQ(root.child(2)->child(3), c4New);
        ASSERT_EQ(c3New->title(), "e1-copy1.txt");
        ASSERT_EQ(c4New->title(), "e1-copy-copy.txt");

        ASSERT_EQ(root.child(3), c1New);
        ASSERT_EQ(root.child(4), c2New);
        ASSERT_EQ(c1New->title(), "e1-copy");
        ASSERT_EQ(c2New->title(), "e1-copy.txt");
    }

    TEST(PboNodeTest, AddEntry2_Throws_In_Case_Of_Conflict) {
        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
    
        root.addEntry(PboPath("f2/e1.txt"));
        const QPointer<PboNode> e1Old = root.child(0)->child(0);
    
        ASSERT_THROW(root.addEntry(PboPath("f2/e1.txt"), TreeConflictResolution::Throw), PboTreeException);
    
        ASSERT_EQ(root.child(0)->childCount(), 1);
        ASSERT_EQ(root.child(0)->child(0), e1Old);
    }

    TEST(PboNodeTest, MakePath_Returns_Path) {
        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(PboPath("e1"));
        root.addEntry(PboPath("f2/e2"));
        root.addEntry(PboPath("f2/e3"));

        ASSERT_EQ(root.makePath().length(), 0);
        ASSERT_THAT(root.child(0)->makePath(), testing::ElementsAre("e1"));
        ASSERT_THAT(root.child(1)->makePath(), testing::ElementsAre("f2"));
        ASSERT_THAT(root.child(1)->child(0)->makePath(), testing::ElementsAre("f2", "e2"));
        ASSERT_THAT(root.child(1)->child(1)->makePath(), testing::ElementsAre("f2", "e3"));
    }

    TEST(PboNodeTest, FileExists_Returns_Correct_Results) {
        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(PboPath("e1"));
        root.addEntry(PboPath("f2/e2"));

        ASSERT_TRUE(root.fileExists(PboPath("e1")));
        ASSERT_FALSE(root.fileExists(PboPath("f2")));
        ASSERT_FALSE(root.fileExists(PboPath("f2/e3")));
    }

    TEST(PboNodeTest, Get_Returns_Node) {
        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(PboPath("e1"));
        root.addEntry(PboPath("f2/e2"));
        root.addEntry(PboPath("f2/e3"));

        ASSERT_EQ(root.get(PboPath{ {"e1"}}), root.child(0));
        ASSERT_EQ(root.get(PboPath{ {"f2"}}), root.child(1));
        ASSERT_EQ(root.get(PboPath{ {"f2", "e2"}}), root.child(1)->child(0));
        ASSERT_EQ(root.get(PboPath{ {"f2", "e3"}}), root.child(1)->child(1));
        ASSERT_FALSE(root.get(PboPath{ {"not-existing"}}));
    }

    TEST(PboNodeTest, RenameNode_Renames) {
        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(PboPath("e1"));
        root.addEntry(PboPath("e2"));

        root.renameNode(PboPath{{"e1"}}, "e3");

        ASSERT_EQ(root.childCount(), 2);
        ASSERT_EQ(root.child(0)->title(), "e3");
        ASSERT_EQ(root.child(1)->title(), "e2");
    }

    TEST(PboNodeTest, RenameNode_Does_Not_Rename_If_Conflict) {
        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(PboPath("e1"));
        root.addEntry(PboPath("e2"));

        root.renameNode(PboPath{{"e1"}}, "e2");

        ASSERT_EQ(root.childCount(), 2);
        ASSERT_EQ(root.child(0)->title(), "e1");
        ASSERT_EQ(root.child(1)->title(), "e2");
    }

    TEST(PboNodeTest, RemoveNode_Removes) {
        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(PboPath("e1"));
        root.addEntry(PboPath("f2/e2"));
        root.addEntry(PboPath("f2/e3"));

        root.removeNode(PboPath{{"e1"}});
        ASSERT_EQ(root.childCount(), 1);
        ASSERT_EQ(root.child(0)->title(), "f2");
        ASSERT_EQ(root.child(0)->childCount(), 2);

        root.removeNode(PboPath{{"f2", "e2"}});
        ASSERT_EQ(root.childCount(), 1);
        ASSERT_EQ(root.child(0)->title(), "f2");
        ASSERT_EQ(root.child(0)->childCount(), 1);
        ASSERT_EQ(root.child(0)->child(0)->title(), "e3");

        root.removeNode(PboPath{{"f2", "e3"}});
        ASSERT_EQ(root.childCount(), 0);
    }

    TEST(PboNodeTest, Get_Returns_Root) {
        PboNode root("file.pbo", PboNodeType::Container, nullptr, nullptr);
        const QPointer<PboNode> node = root.get(PboPath());
        ASSERT_EQ(node.get(), &root);
    }
}
