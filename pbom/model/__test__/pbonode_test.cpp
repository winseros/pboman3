#include "model/pbonode.h"
#include <QPointer>
#include <QTemporaryFile>
#include <gtest/gtest.h>
#include "gmock/gmock.h"
#include "util/exception.h"

namespace pboman3::test {
    TEST(PboNodeTest, Ctor_Initializes_Node) {
        PboNode nodeA("a-node", PboNodeType::Folder, nullptr);
        ASSERT_EQ(nodeA.nodeType(), PboNodeType::Folder);
        ASSERT_EQ(nodeA.title(), "a-node");
        ASSERT_FALSE(nodeA.parentNode());

        const PboNode nodeB("b-node", PboNodeType::File, &nodeA);
        ASSERT_EQ(nodeB.nodeType(), PboNodeType::File);
        ASSERT_EQ(nodeB.title(), "b-node");
        ASSERT_EQ(nodeB.parentNode(), &nodeA);
    }

    TEST(PboNodeTest, CreateHierarchy1_Creates_Tree) {
        PboNode root("file-name", PboNodeType::Container, nullptr);
        ASSERT_EQ(root.depth(), 0);

        root.createHierarchy(PboPath("e1"));
        root.createHierarchy(PboPath("f2/e2"));
        root.createHierarchy(PboPath("f2/e3"));

        //f2
        ASSERT_EQ(root.count(), 2);
        ASSERT_EQ(root.at(0)->nodeType(), PboNodeType::Folder);
        ASSERT_EQ(root.at(0)->title(), "f2");
        ASSERT_EQ(root.at(0)->parentNode(), &root);
        ASSERT_EQ(root.at(0)->depth(), 1);

        //e1
        ASSERT_EQ(root.at(1)->nodeType(), PboNodeType::File);
        ASSERT_EQ(root.at(1)->title(), "e1");
        ASSERT_EQ(root.at(1)->parentNode(), &root);
        ASSERT_EQ(root.at(1)->depth(), 1);

        //f2/e2
        ASSERT_EQ(root.at(0)->count(), 2);
        ASSERT_EQ(root.at(0)->at(0)->nodeType(), PboNodeType::File);
        ASSERT_EQ(root.at(0)->at(0)->title(), "e2");
        ASSERT_EQ(root.at(0)->at(0)->parentNode(), root.at(0));
        ASSERT_EQ(root.at(0)->at(0)->depth(), 2);

        //f2/e3
        ASSERT_EQ(root.at(0)->at(1)->nodeType(), PboNodeType::File);
        ASSERT_EQ(root.at(0)->at(1)->title(), "e3");
        ASSERT_EQ(root.at(0)->at(1)->parentNode(), root.at(0));
        ASSERT_EQ(root.at(0)->at(1)->depth(), 2);
    }

    TEST(PboNodeTest, CreateHierarchy1_Emits_Once_When_Creating_Folders) {
        PboNode root("file-name", PboNodeType::Container, nullptr);

        int count = 0;
        auto onChildCreated = [&count](PboNode* node, qsizetype index) {
            ASSERT_EQ(node->title(), "f1");
            ASSERT_EQ(index, 0);
            count++;
        };

        QObject::connect(&root, &PboNode::childCreated, onChildCreated);

        root.createHierarchy(PboPath("f1/e1"));
        root.createHierarchy(PboPath("f1/e2"));

        ASSERT_EQ(count, 1);
    }

    TEST(PboNodeTest, CreateHierarchy1_Emits_When_Creating_Files) {
        PboNode root("file-name", PboNodeType::Container, nullptr);

        int count = 0;
        auto onChildCreated = [&count](PboNode* node, qsizetype index) {
            switch (count) {
                case 0: {
                    ASSERT_EQ(node->title(), "e1");
                    ASSERT_EQ(index, 0);
                    break;
                }
                case 1: {
                    ASSERT_EQ(node->title(), "e2");
                    ASSERT_EQ(index, 1);
                    break;
                }
                default: assert(false);
            }
            count++;
        };

        QObject::connect(&root, &PboNode::childCreated, onChildCreated);

        root.createHierarchy(PboPath("e1"));
        root.createHierarchy(PboPath("e2"));

        ASSERT_EQ(count, 2);
    }

    TEST(PboNodeTest, CreateHierarchy2_Creates_Tree_If_Entries_Make_No_Conflicts) {
        PboNode root("file-name", PboNodeType::Container, nullptr);

        PboNode* e1 = root.createHierarchy(PboPath("e1"), ConflictResolution::Unset);
        PboNode* e2 = root.createHierarchy(PboPath("f2/e2"), ConflictResolution::Unset);
        PboNode* e3 = root.createHierarchy(PboPath("f2/e3"), ConflictResolution::Unset);

        ASSERT_EQ(root.count(), 2);

        //f2 check
        ASSERT_EQ(root.at(0)->nodeType(), PboNodeType::Folder);
        ASSERT_EQ(root.at(0)->title(), "f2");
        ASSERT_EQ(root.at(0)->parentNode(), &root);
        ASSERT_EQ(root.at(0)->count(), 2);

        //f2/e2
        ASSERT_EQ(root.at(0)->at(0), e2);
        ASSERT_EQ(root.at(0)->at(0)->nodeType(), PboNodeType::File);
        ASSERT_EQ(root.at(0)->at(0)->title(), "e2");
        ASSERT_EQ(root.at(0)->at(0)->parentNode(), root.at(0));

        //f2/e3
        ASSERT_EQ(root.at(0)->at(1), e3);
        ASSERT_EQ(root.at(0)->at(1)->nodeType(), PboNodeType::File);
        ASSERT_EQ(root.at(0)->at(1)->title(), "e3");
        ASSERT_EQ(root.at(0)->at(1)->parentNode(), root.at(0));

        //e1 check
        ASSERT_EQ(root.at(1), e1);
        ASSERT_EQ(root.at(1)->nodeType(), PboNodeType::File);
        ASSERT_EQ(root.at(1)->title(), "e1");
        ASSERT_EQ(root.at(1)->parentNode(), &root);
    }

    TEST(PboNodeTest, CreateHierarchy2_Replaces_Conflicting_Node) {
        PboNode root("file-name", PboNodeType::Container, nullptr);

        root.createHierarchy(PboPath("f2/e1"));
        const QPointer<PboNode> e1Old = root.at(0)->at(0);

        PboNode* e1New = root.createHierarchy(PboPath("f2/e1"), ConflictResolution::Replace);

        ASSERT_EQ(root.at(0)->count(), 1);
        ASSERT_EQ(root.at(0)->at(0), e1New);
        ASSERT_TRUE(e1Old.isNull());
    }

    TEST(PboNodeTest, CreateHierarchy2_Renames_Conflicting_Node) {
        PboNode root("file-name", PboNodeType::Container, nullptr);

        PboNode* c1Old = root.createHierarchy(PboPath("e1"));
        PboNode* c2Old = root.createHierarchy(PboPath("e1.txt"));
        PboNode* c3Old = root.createHierarchy(PboPath("f2/e1.txt"));
        PboNode* c4Old = root.createHierarchy(PboPath("f2/e1-copy.txt"));

        PboNode* c1New = root.createHierarchy(PboPath("e1"), ConflictResolution::Copy);
        PboNode* c2New = root.createHierarchy(PboPath("e1.txt"), ConflictResolution::Copy);
        PboNode* c3New = root.createHierarchy(PboPath("f2/e1.txt"), ConflictResolution::Copy);
        PboNode* c4New = root.createHierarchy(PboPath("f2/e1-copy.txt"), ConflictResolution::Copy);

        ASSERT_EQ(root.count(), 5);
        ASSERT_EQ(root.at(1), c1Old);
        ASSERT_EQ(root.at(4), c2Old);

        ASSERT_EQ(root.at(2), c1New);
        ASSERT_EQ(root.at(3), c2New);
        ASSERT_EQ(c1New->title(), "e1-copy");
        ASSERT_EQ(c2New->title(), "e1-copy.txt");

        ASSERT_EQ(root.at(0)->count(), 4);
        ASSERT_EQ(root.at(0)->at(0), c4New);
        ASSERT_EQ(root.at(0)->at(1), c4Old);
        ASSERT_EQ(root.at(0)->at(2), c3New);
        ASSERT_EQ(root.at(0)->at(3), c3Old);
        ASSERT_EQ(c3New->title(), "e1-copy1.txt");
        ASSERT_EQ(c4New->title(), "e1-copy-copy.txt");
    }

    TEST(PboNodeTest, CreateHierarchy2_Throws_In_Case_Of_Conflict) {
        PboNode root("file-name", PboNodeType::Container, nullptr);

        root.createHierarchy(PboPath("f2/e1.txt"));
        PboNode* e1Old = root.at(0)->at(0);

        ASSERT_THROW(root.createHierarchy(PboPath("f2/e1.txt"), ConflictResolution::Unset), InvalidOperationException);
        ASSERT_THROW(root.createHierarchy(PboPath("f2/e1.txt"), ConflictResolution::Skip), InvalidOperationException);

        ASSERT_EQ(root.at(0)->count(), 1);
        ASSERT_EQ(root.at(0)->at(0), e1Old);
    }

    TEST(PboNodeTest, CreateHierarchy2_Emits_HierarchyChanged_On_Root) {
        PboNode root("file-name", PboNodeType::Container, nullptr);
        root.createHierarchy(PboPath("f1/e2"));

        int count = 0;
        auto hierarchyChanged1 = []() { ASSERT_FALSE("Should not have been called"); };
        auto hierarchyChanged2 = [&count]() {count++; };

        QObject::connect(root.at(0), &PboNode::hierarchyChanged, hierarchyChanged1);
        QObject::connect(&root, &PboNode::hierarchyChanged, hierarchyChanged2);

        //only the root fires the callback
        root.createHierarchy(PboPath("f1/e1"), ConflictResolution::Unset);
        ASSERT_EQ(count, 1);

        root.createHierarchy(PboPath("f1/e2"), ConflictResolution::Replace);
        ASSERT_EQ(count, 2);

        root.createHierarchy(PboPath("f1/e2"), ConflictResolution::Copy);
        ASSERT_EQ(count, 3);
    }

    TEST(PboNodeTest, MakePath_Returns_Path) {
        PboNode root("file-name", PboNodeType::Container, nullptr);
        root.createHierarchy(PboPath("e1"));
        root.createHierarchy(PboPath("f2/e2"));
        root.createHierarchy(PboPath("f2/e3"));

        ASSERT_EQ(root.makePath().length(), 0);
        ASSERT_THAT(root.at(1)->makePath(), testing::ElementsAre("e1"));
        ASSERT_THAT(root.at(0)->makePath(), testing::ElementsAre("f2"));
        ASSERT_THAT(root.at(0)->at(0)->makePath(), testing::ElementsAre("f2", "e2"));
        ASSERT_THAT(root.at(0)->at(1)->makePath(), testing::ElementsAre("f2", "e3"));
    }

    TEST(PboNodeTest, Get_Returns_Node) {
        PboNode root("file-name", PboNodeType::Container, nullptr);
        root.createHierarchy(PboPath("e1"));
        root.createHierarchy(PboPath("f2/e2"));
        root.createHierarchy(PboPath("f2/e3"));

        ASSERT_EQ(root.get(PboPath("e1")), root.at(1));
        ASSERT_EQ(root.get(PboPath("f2")), root.at(0));
        ASSERT_EQ(root.get(PboPath("f2/e2")), root.at(0)->at(0));
        ASSERT_EQ(root.get(PboPath("f2/e3")), root.at(0)->at(1));
        ASSERT_FALSE(root.get(PboPath{ {"not-existing"}}));
    }

    TEST(PboNodeTest, Get_Returns_Self) {
        PboNode root("file.pbo", PboNodeType::Container, nullptr);
        PboNode* node = root.get(PboPath());
        ASSERT_EQ(node, &root);
    }

    TEST(PboNodeTest, SetTitle_Throws_If_Can_Not_Set_Title) {
        PboNode root("file-name", PboNodeType::Container, nullptr);
        PboNode* e1 = root.createHierarchy(PboPath("e1"));
        root.createHierarchy(PboPath("e2"));

        ASSERT_THROW(e1->setTitle("e2"), InvalidOperationException);
    }

    TEST(PboNodeTest, SetTitle_Emits_If_Set_Title) {
        PboNode root("file-name", PboNodeType::Container, nullptr);
        PboNode* e1 = root.createHierarchy(PboPath("e1"));

        //callback variables
        int count = 0;

        //connect and wait for the callback
        QObject::connect(e1, &PboNode::titleChanged, [&count](const QString& title) {
            ASSERT_EQ(title, "e2");
            count++;
        });

        e1->setTitle("e2");
        ASSERT_EQ(count, 1);
    }

    TEST(PboNodeTest, SetTitle_Does_Not_Emit_If_Did_Not_Set_Title) {
        PboNode root("file-name", PboNodeType::Container, nullptr);
        PboNode* e1 = root.createHierarchy(PboPath("e1"));

        //callback variables
        int callbackCount = 0;

        //connect and wait for the callback
        QObject::connect(e1, &PboNode::titleChanged, [&callbackCount]() {
            callbackCount++;
        });

        e1->setTitle("e1"); //the same name - not changing

        ASSERT_EQ(callbackCount, 0);
    }

    TEST(PboNodeTest, SetTitle_Emits_HierarchyChanged_On_Root) {
        PboNode root("file-name", PboNodeType::Container, nullptr);
        PboNode* e2 = root.createHierarchy(PboPath("f1/e2"));

        int count = 0;
        auto hierarchyChanged1 = []() { ASSERT_FALSE("Should not have been called"); };
        auto hierarchyChanged2 = [&count]() {count++; };

        QObject::connect(root.at(0), &PboNode::hierarchyChanged, hierarchyChanged1);
        QObject::connect(&root, &PboNode::hierarchyChanged, hierarchyChanged2);

        //only the root fires the callback
        e2->setTitle("e1");
        ASSERT_EQ(count, 1);

        e2->setTitle("e1");//does not emit if no actual change
        ASSERT_EQ(count, 1);
    }

    struct VerifyTitleTestParam {
        QString input;
        QString expectedOutput;
    };

    class VerifyTitleTest : public testing::TestWithParam<VerifyTitleTestParam> {
    };

    TEST_P(VerifyTitleTest, VerifyTitle_Is_Functional) {
        PboNode root("file-name", PboNodeType::Container, nullptr);
        root.createHierarchy(PboPath("e1"));
        PboNode* e2 = root.createHierarchy(PboPath("e2"));

        ASSERT_EQ(e2->verifyTitle(GetParam().input), GetParam().expectedOutput);
    }

    INSTANTIATE_TEST_SUITE_P(PboNodeTest, VerifyTitleTest, testing::Values(
                                 VerifyTitleTestParam {nullptr, "The value can not be empty"},
                                 VerifyTitleTestParam {"", "The value can not be empty"},
                                 VerifyTitleTestParam {"e1", "The item with this name already exists"},
                                 VerifyTitleTestParam {"e2", ""}));

    TEST(PboNodeTest, RemoveFromHierarchy_Removes) {
        PboNode root("file-name", PboNodeType::Container, nullptr);
        PboNode* e1 = root.createHierarchy(PboPath("e1"));
        PboNode* e2 = root.createHierarchy(PboPath("f2/e2"));
        PboNode* e3 = root.createHierarchy(PboPath("f2/e3"));

        e1->removeFromHierarchy();
        ASSERT_EQ(root.count(), 1);
        ASSERT_EQ(root.at(0)->title(), "f2");
        ASSERT_EQ(root.at(0)->count(), 2);

        e2->removeFromHierarchy();
        ASSERT_EQ(root.count(), 1);
        ASSERT_EQ(root.at(0)->title(), "f2");
        ASSERT_EQ(root.at(0)->count(), 1);
        ASSERT_EQ(root.at(0)->at(0)->title(), "e3");

        e3->removeFromHierarchy();
        ASSERT_EQ(root.count(), 0);
    }

    TEST(PboNodeTest, RemoveFromHierarchy_Throws_If_Can_Not_Remove) {
        PboNode root("file-name", PboNodeType::Container, nullptr);

        ASSERT_THROW(root.removeFromHierarchy(), InvalidOperationException);
    }

    TEST(PboNodeTest, RemoveFromHierarchy_Emits_Events) {
        PboNode root("file-name", PboNodeType::Container, nullptr);
        PboNode* e1 = root.createHierarchy(PboPath("f1/e1"));
        PboNode* e2 = root.createHierarchy(PboPath("f1/e2"));
        PboNode* e3 = root.createHierarchy(PboPath("f1/e3"));

        int countF1 = 0;
        auto onRemovedF1 = [&countF1](qsizetype index) {
            switch (countF1) {
                case 0: {
                    ASSERT_EQ(index, 2); //e3 removed
                    break;
                }
                case 1: {
                    ASSERT_EQ(index, 1); //e2 removed
                    break;
                }
                default: assert(false);
            }
            countF1++;
        };

        int countR = 0;
        auto onRemovedR = [&countR](qsizetype index) {
            ASSERT_EQ(index, 0); //e1 removed
            countR++;
        };

        QObject::connect(&root, &PboNode::childRemoved, onRemovedR);
        QObject::connect(root.at(0), &PboNode::childRemoved, onRemovedF1);

        e3->removeFromHierarchy();
        ASSERT_EQ(countF1, 1);
        ASSERT_EQ(countR, 0);

        e2->removeFromHierarchy();
        ASSERT_EQ(countF1, 2);
        ASSERT_EQ(countR, 0);

        e1->removeFromHierarchy();
        ASSERT_EQ(countF1, 2);
        ASSERT_EQ(countR, 1);
    }

    TEST(PboNodeTest, RemoveFromHierarchy_Emits_HierarchyChanged_On_Root) {
        PboNode root("file-name", PboNodeType::Container, nullptr);
        root.createHierarchy(PboPath("f1/e2"));
        PboNode* e3 = root.createHierarchy(PboPath("f1/e3"));

        int count = 0;
        auto hierarchyChanged1 = []() { ASSERT_FALSE("Should not have been called"); };
        auto hierarchyChanged2 = [&count]() {count++; };

        QObject::connect(root.at(0), &PboNode::hierarchyChanged, hierarchyChanged1);
        QObject::connect(&root, &PboNode::hierarchyChanged, hierarchyChanged2);

        //only the root fires the callback
        e3->removeFromHierarchy();
        ASSERT_EQ(count, 1);
    }
}
