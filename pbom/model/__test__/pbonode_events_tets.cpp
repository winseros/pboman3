#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "model/pbonode.h"
#include "model/pbonodetype.h"
#include "model/pbotreeexception.h"

namespace pboman3::test {
    TEST(PboNodeEventsTest, AddEntry1_Emits) {
        auto i = 0;
        const auto onEvent = [&i](const PboNodeEvent* evt) {
            i++;
            if (i == 1) {
                const auto* evtE1 = dynamic_cast<const PboNodeCreatedEvent*>(evt);
                ASSERT_TRUE(evtE1);
                ASSERT_THAT(*evtE1->nodePath, testing::ElementsAre("e1"));
                ASSERT_EQ(evtE1->nodeType, PboNodeType::File);
            } else if (i == 2) {
                const auto* evtF2 = dynamic_cast<const PboNodeCreatedEvent*>(evt);
                ASSERT_TRUE(evtF2);
                ASSERT_THAT(*evtF2->nodePath, testing::ElementsAre("f2"));
                ASSERT_EQ(evtF2->nodeType, PboNodeType::Folder);
            } else if (i == 3) {
                const auto* evtE2 = dynamic_cast<const PboNodeCreatedEvent*>(evt);
                ASSERT_TRUE(evtE2);
                ASSERT_THAT(*evtE2->nodePath, testing::ElementsAre("f2", "e2"));
                ASSERT_EQ(evtE2->nodeType, PboNodeType::File);
            } else if (i == 4) {
                const auto* evtE3 = dynamic_cast<const PboNodeCreatedEvent*>(evt);
                ASSERT_TRUE(evtE3);
                ASSERT_THAT(*evtE3->nodePath, testing::ElementsAre("f2", "e3"));
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

    TEST(PboNodeEventsTest, AddEntry2_Emits_If_Entries_Make_No_Conflicts) {
        auto i = 0;
        const auto onEvent = [&i](const PboNodeEvent* evt) {
            i++;
            if (i == 1) {
                const auto* evtE1 = dynamic_cast<const PboNodeCreatedEvent*>(evt);
                ASSERT_TRUE(evtE1);
                ASSERT_THAT(*evtE1->nodePath, testing::ElementsAre("e1"));
                ASSERT_EQ(evtE1->nodeType, PboNodeType::File);
            } else if (i == 2) {
                const auto* evtF2 = dynamic_cast<const PboNodeCreatedEvent*>(evt);
                ASSERT_TRUE(evtF2);
                ASSERT_THAT(*evtF2->nodePath, testing::ElementsAre("f2"));
                ASSERT_EQ(evtF2->nodeType, PboNodeType::Folder);
            } else if (i == 3) {
                const auto* evtE2 = dynamic_cast<const PboNodeCreatedEvent*>(evt);
                ASSERT_TRUE(evtE2);
                ASSERT_THAT(*evtE2->nodePath, testing::ElementsAre("f2", "e2"));
                ASSERT_EQ(evtE2->nodeType, PboNodeType::File);
            } else if (i == 4) {
                const auto* evtE3 = dynamic_cast<const PboNodeCreatedEvent*>(evt);
                ASSERT_TRUE(evtE3);
                ASSERT_THAT(*evtE3->nodePath, testing::ElementsAre("f2", "e3"));
                ASSERT_EQ(evtE3->nodeType, PboNodeType::File);
            }
        };

        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        QObject::connect(&root, &PboNode::onEvent, onEvent);

        root.addEntry(PboPath("e1"), ConflictResolution::Unset);
        root.addEntry(PboPath("f2/e2"), ConflictResolution::Unset);
        root.addEntry(PboPath("f2/e3"), ConflictResolution::Unset);

        ASSERT_EQ(i, 4);
    }

    TEST(PboNodeEventsTest, AddEntry2_Emits_When_Replacing_Conflicting_Entry) {
        auto i = 0;
        const auto onEvent = [&i](const PboNodeEvent* evt) {
            i++;
            if (i == 1) {
                const auto* evtE1 = dynamic_cast<const PboNodeRemovedEvent*>(evt);
                ASSERT_TRUE(evtE1);
                ASSERT_THAT(*evtE1->nodePath, testing::ElementsAre("f2", "e2"));
            } else if (i == 2) {
                const auto* evtF2 = dynamic_cast<const PboNodeCreatedEvent*>(evt);
                ASSERT_TRUE(evtF2);
                ASSERT_THAT(*evtF2->nodePath, testing::ElementsAre("f2", "e2"));
                ASSERT_EQ(evtF2->nodeType, PboNodeType::File);
            }
        };

        //initial root setup
        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(PboPath("f2/e2"));

        //add a conflicting entry
        QObject::connect(&root, &PboNode::onEvent, onEvent);
        root.addEntry(PboPath("f2/e2"), ConflictResolution::Replace);

        ASSERT_EQ(i, 2);
    }

    TEST(PboNodeEventsTest, AddEntry2_Emits_When_Copying_Conflicting_Entry) {
        auto i = 0;
        const auto onEvent = [&i](const PboNodeEvent* evt) {
            i++;
            if (i == 1) {
                const auto* evtF2 = dynamic_cast<const PboNodeCreatedEvent*>(evt);
                ASSERT_TRUE(evtF2);
                ASSERT_THAT(*evtF2->nodePath, testing::ElementsAre("f2", "e2-copy"));
                ASSERT_EQ(evtF2->nodeType, PboNodeType::File);
            }
        };

        //initial root setup
        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(PboPath("f2/e2"));

        //add a conflicting entry
        QObject::connect(&root, &PboNode::onEvent, onEvent);
        root.addEntry(PboPath("f2/e2"), ConflictResolution::Copy);

        ASSERT_EQ(i, 1);
    }

    TEST(PboNodeEventsTest, AddEntry2_Not_Emits_When_Throwing_On_Conflicting_Entry) {
        auto i = 0;
        const auto onEvent = [&i](const PboNodeEvent*) {
            i++;
        };
    
        //initial root setup
        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(PboPath("f2/e2"));
    
        //add a conflicting entry
        QObject::connect(&root, &PboNode::onEvent, onEvent);
        ASSERT_THROW(root.addEntry(PboPath("f2/e2"), ConflictResolution::Unset), PboTreeException);
        ASSERT_THROW(root.addEntry(PboPath("f2/e2"), ConflictResolution::Skip), PboTreeException);
    
        ASSERT_EQ(i, 0);
    }

    TEST(PboNodeEventsTest, RemoveEntry_Emits) {
        auto i = 0;
        const auto onEvent = [&i](const PboNodeEvent* evt) {
            i++;
            if (i == 1) {
                const auto* evtE1 = dynamic_cast<const PboNodeRemovedEvent*>(evt);
                ASSERT_TRUE(evtE1);
                ASSERT_THAT(*evtE1->nodePath, testing::ElementsAre("e1"));
            } else if (i == 2) {
                const auto* evtF2 = dynamic_cast<const PboNodeRemovedEvent*>(evt);
                ASSERT_TRUE(evtF2);
                ASSERT_THAT(*evtF2->nodePath, testing::ElementsAre("f2", "e2"));
            } else if (i == 3) {
                const auto* evtE2 = dynamic_cast<const PboNodeRemovedEvent*>(evt);
                ASSERT_TRUE(evtE2);
                ASSERT_THAT(*evtE2->nodePath, testing::ElementsAre("f2"));
            }
        };

        PboNode root("file-name", PboNodeType::Container, nullptr, nullptr);
        root.addEntry(PboPath("e1"));
        root.addEntry(PboPath("f2/e2"));
        root.addEntry(PboPath("f2/e3"));

        QObject::connect(&root, &PboNode::onEvent, onEvent);

        root.removeNode(PboPath("e1"));
        root.removeNode(PboPath("f2/e2"));
        root.removeNode(PboPath("f2/e3"));

        ASSERT_EQ(i, 3);
    }
}
