#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "model/pbonode.h"
#include "model/pbonodetype.h"

namespace pboman3::test {
    TEST(PboNodeEventsTest, AddEntry_Emits) {
        auto i = 0;
        const auto onEvent = [&i](const PboNodeEvent* evt) {
            i++;
            if (i == 1) {
                const auto* evtE1 = dynamic_cast<const PboNodeCreatedEvent*>(evt);
                ASSERT_TRUE(evtE1);
                ASSERT_THAT(*evtE1->path, testing::ElementsAre("e1"));
                ASSERT_EQ(evtE1->nodeType, PboNodeType::File);
            } else if (i == 2) {
                const auto* evtF2 = dynamic_cast<const PboNodeCreatedEvent*>(evt);
                ASSERT_TRUE(evtF2);
                ASSERT_THAT(*evtF2->path, testing::ElementsAre("f2"));
                ASSERT_EQ(evtF2->nodeType, PboNodeType::Folder);
            } else if (i == 3) {
                const auto* evtE2 = dynamic_cast<const PboNodeCreatedEvent*>(evt);
                ASSERT_TRUE(evtE2);
                ASSERT_THAT(*evtE2->path, testing::ElementsAre("f2", "e2"));
                ASSERT_EQ(evtE2->nodeType, PboNodeType::File);
            } else if (i == 4) {
                const auto* evtE3 = dynamic_cast<const PboNodeCreatedEvent*>(evt);
                ASSERT_TRUE(evtE3);
                ASSERT_THAT(*evtE3->path, testing::ElementsAre("f2", "e3"));
                ASSERT_EQ(evtE3->nodeType, PboNodeType::File);
            }
        };

        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        QObject::connect(&root, &PboNode::onEvent, onEvent);

        root.addEntry(PboPath("e1"));
        root.addEntry(PboPath("f2/e2"));
        root.addEntry(PboPath("f2/e3"));

        ASSERT_EQ(i, 4);
    }

    TEST(PboNodeTest, MoveNode_Emits_When_No_Conflicts) {
        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(PboPath("e1"));
        root.addEntry(PboPath("f2/e2"));
        root.addEntry(PboPath("f3/e3"));

        auto i = 0;
        const auto onEvent = [&i](const PboNodeEvent* evt) {
            i++;
            if (i == 1) {
                const auto* evtE1 = dynamic_cast<const PboNodeMovedEvent*>(evt);
                ASSERT_TRUE(evtE1);
                ASSERT_THAT(*evtE1->newNodePath, testing::ElementsAre("f2", "e1"));
            } else if (i == 2) {
                const auto* evtE3 = dynamic_cast<const PboNodeMovedEvent*>(evt);
                ASSERT_TRUE(evtE3);
                ASSERT_THAT(*evtE3->newNodePath, testing::ElementsAre("e3"));
            } else if (i == 3) {
                const auto* evtF3 = dynamic_cast<const PboNodeRemovedEvent*>(evt);
                ASSERT_TRUE(evtF3);
                ASSERT_THAT(*evtF3->nodePath, testing::ElementsAre("f3"));
            }
        };
        QObject::connect(&root, &PboNode::onEvent, onEvent);

        root.moveNode(PboPath{{"e1"}}, PboPath{{"f2"}}, nullptr);
        ASSERT_EQ(i, 1);

        root.moveNode(PboPath{{"f3", "e3"}}, PboPath(), nullptr);
        ASSERT_EQ(i, 3);
    }

    TEST(PboNodeTest, MoveNode_Emits_If_Conflict_Resolved_In_Rename) {
        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(PboPath("e1"));
        root.addEntry(PboPath("f2/e1"));

        auto i = 0;
        const auto onEvent = [&i](const PboNodeEvent* evt) {
            i++;
            if (i == 1) {
                const auto* evtE1 = dynamic_cast<const PboNodeMovedEvent*>(evt);
                ASSERT_TRUE(evtE1);
                ASSERT_THAT(*evtE1->newNodePath, testing::ElementsAre("f2", "e1-copy"));
                ASSERT_THAT(*evtE1->prevNodePath, testing::ElementsAre("e1"));
            }
        };
        QObject::connect(&root, &PboNode::onEvent, onEvent);

        const auto onConflict = [](const PboPath&, const PboNodeType) { return PboConflictResolution::Copy; };
        root.moveNode(PboPath{{"e1"}}, PboPath{{"f2"}}, onConflict);
        ASSERT_EQ(i, 1);
    }

    TEST(PboNodeTest, MoveNode_Emits_If_Conflict_Resolved_In_Replace) {
        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(PboPath("e1"));
        root.addEntry(PboPath("f2/e1"));

        auto i = 0;
        const auto onEvent = [&i](const PboNodeEvent* evt) {
            i++;
            if (i == 1) {
                const auto* evtE1 = dynamic_cast<const PboNodeRemovedEvent*>(evt);
                ASSERT_TRUE(evtE1);
                ASSERT_THAT(*evtE1->nodePath, testing::ElementsAre("f2", "e1"));
            } else if (i == 2) {
                const auto* evtE1 = dynamic_cast<const PboNodeMovedEvent*>(evt);
                ASSERT_TRUE(evtE1);
                ASSERT_THAT(*evtE1->newNodePath, testing::ElementsAre("f2", "e1"));
                ASSERT_THAT(*evtE1->prevNodePath, testing::ElementsAre("e1"));
            }
        };
        QObject::connect(&root, &PboNode::onEvent, onEvent);

        const auto onConflict = [](const PboPath&, const PboNodeType) { return PboConflictResolution::ReplaceOrMerge; };
        root.moveNode(PboPath{{"e1"}}, PboPath{{"f2"}}, onConflict);
        ASSERT_EQ(i, 2);
    }

    TEST(PboNodeTest, MoveNode_Emits_If_Conflict_Resolved_In_Merge) {
        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(PboPath("f1/e1"));
        root.addEntry(PboPath("f2/f1/e2"));

        auto i = 0;
        const auto onEvent = [&i](const PboNodeEvent* evt) {
            i++;
            if (i == 1) {
                const auto* evtE1 = dynamic_cast<const PboNodeMovedEvent*>(evt);
                ASSERT_TRUE(evtE1);
                ASSERT_THAT(*evtE1->newNodePath, testing::ElementsAre("f2", "f1", "e1"));
                ASSERT_THAT(*evtE1->prevNodePath, testing::ElementsAre("f1", "e1"));
            } else if (i == 2) {
                const auto* evtE2 = dynamic_cast<const PboNodeRemovedEvent*>(evt);
                ASSERT_TRUE(evtE2);
                ASSERT_THAT(*evtE2->nodePath, testing::ElementsAre("f1"));
            }
        };
        QObject::connect(&root, &PboNode::onEvent, onEvent);

        const auto onConflict = [](const PboPath&, const PboNodeType) { return PboConflictResolution::ReplaceOrMerge; };
        root.moveNode(PboPath{{"f1"}}, PboPath{{"f2"}}, onConflict);
        ASSERT_EQ(i, 2);
    }

    TEST(PboNodeTest, MoveNode_Not_Emits_If_Conflict_Resolved_In_Abort) {
        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(PboPath("e1"));
        root.addEntry(PboPath("f2/e1"));

        auto i = 0;
        const auto onEvent = [&i](const PboNodeEvent* evt) {
            i++;
        };
        QObject::connect(&root, &PboNode::onEvent, onEvent);

        const auto onConflict = [](const PboPath&, const PboNodeType) { return PboConflictResolution::Abort; };
        root.moveNode(PboPath{{"e1"}}, PboPath{{"f2"}}, onConflict);
        ASSERT_EQ(i, 0);
    }
}
