#include <QApplication>

#include <gtest/gtest.h>
#include "ui/treewidget/treewidgetbase.h"

namespace pboman3::ui::test {
    namespace treewidgetbase_test {
        class MockTreeWidgetBase : public TreeWidgetBase {
        public:
            PboNode* getSelectionRootMock() const {
                return getSelectionRoot();
            }

            QList<PboNode*> getSelectedHierarchiesMock() const {
                return getSelectedHierarchies();
            }

        protected:
            void dragStarted(const QList<PboNode*>& items) override {
            }

            void dragDropped(PboNode* target, const QMimeData* mimeData) override {
            }

        };

        class MockTreeWidgetBaseIconMgr : public IconMgr {
        public:
            const QIcon& getIconForExtension(const QString& extension) override {
                return icon_;
            }

            const QIcon& getFolderOpenedIcon() override {
                return icon_;
            }

            const QIcon& getFolderClosedIcon() override {
                return icon_;
            }

        private:
            QIcon icon_;
        };
    }

    using namespace treewidgetbase_test;

#define QTAPP int argc = 0; \
        char** argv = nullptr; \
        QApplication app(argc, argv);

    TEST(TreeWidgetBaseTest, GetSelectionHierarchies_Returns_When_Single_Node_Selected) {
        QTAPP

        const auto node = QScopedPointer(new PboNode("file.pbo", PboNodeType::Container, nullptr));
        const auto item = new TreeWidgetItem(node.get());

        MockTreeWidgetBase tree;
        tree.addTopLevelItem(item);

        item->setSelected(true);

        ASSERT_EQ(tree.getSelectedHierarchiesMock(), QList({node.get()}));
    }

    TEST(TreeWidgetBaseTest, GetSelectedHierarchies_Returns_Correct_Results_When_Multiple_Items_Selected) {
        QTAPP

        //build the mock tree
        const auto iconMgr = QSharedPointer<IconMgr>(new MockTreeWidgetBaseIconMgr);
        const auto node = QScopedPointer(new PboNode("file.pbo", PboNodeType::Container, nullptr));
        const auto item = new TreeWidgetItem(node.get(), iconMgr);

        //create tree nodes
        node->createHierarchy(PboPath("e1/e2/f1.txt"));
        node->createHierarchy(PboPath("e1/e2/f2.txt"));
        node->createHierarchy(PboPath("e1/e3/f3.txt"));
        node->createHierarchy(PboPath("e1/e3/f4.txt"));

        //att tree to the widget
        MockTreeWidgetBase tree;
        tree.addTopLevelItem(item);

        //set items selected
        item->child(0)->child(0)->child(0)->setSelected(true); //f1.txt
        item->child(0)->child(0)->setSelected(true); //e2
        item->child(0)->child(1)->child(0)->setSelected(true); //f3.txt
        
        //call and assert
        const QList<PboNode*> selected = tree.getSelectedHierarchiesMock();       
        ASSERT_EQ(selected.count(), 2);
        ASSERT_EQ(selected.at(0)->title(), "e2");
        ASSERT_EQ(selected.at(1)->title(), "f3.txt");
    }


    TEST(TreeWidgetBaseTest, GetSelectionRoot_Returns_Null) {
        QTAPP
        const MockTreeWidgetBase tree;
        ASSERT_FALSE(tree.getSelectionRootMock());
    }

    TEST(TreeWidgetBaseTest, GetSelectionRoot_Returns_When_Single_Node_Selected) {
        QTAPP

        const auto node = QScopedPointer(new PboNode("file.pbo", PboNodeType::Container, nullptr));
        const auto item = new TreeWidgetItem(node.get());

        MockTreeWidgetBase tree;
        tree.addTopLevelItem(item);

        item->setSelected(true);

        ASSERT_EQ(tree.getSelectionRootMock(), node.get());
    }

    TEST(TreeWidgetBaseTest, GetSelectionRoot_Returns_Correct_Results_When_Multiple_Items_Selected) {
        QTAPP

        //build the mock tree
        const auto iconMgr = QSharedPointer<IconMgr>(new MockTreeWidgetBaseIconMgr);
        const auto node = QScopedPointer(new PboNode("file.pbo", PboNodeType::Container, nullptr));
        const auto item = new TreeWidgetItem(node.get(), iconMgr);

        //create tree nodes
        node->createHierarchy(PboPath("e1/e2/f1.txt"));
        node->createHierarchy(PboPath("e1/e2/f2.txt"));

        //att tree to the widget
        MockTreeWidgetBase tree;
        tree.addTopLevelItem(item);

        //set items selected
        item->child(0)->child(0)->child(0)->setSelected(true); //f1.txt
        item->child(0)->child(0)->child(1)->setSelected(true); //f2.txt

        //call and assert
        PboNode* selected = tree.getSelectionRootMock();
        ASSERT_TRUE(selected);
        ASSERT_EQ(selected->title(), "e2");
    }
}
