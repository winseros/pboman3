#include "treecontrol.h"

namespace pboman3 {
    TreeControl::TreeControl(QTreeWidget* tree)
        : tree_(tree) {
        root_ = QSharedPointer<TreeWidgetItem>(new TreeWidgetItem(PboNodeType::Container));
    }

    void TreeControl::setNewRoot(const QString& fileName) const {
        tree_->clear();
        root_->setText(0, fileName);
        root_->setIcon(0, QIcon(":ifolderopened.png"));
    }

    void TreeControl::addNewNode(const PboPath& path, PboNodeType nodeType) {
        const PboPath parentPath = path.makeParent();
        QSharedPointer<TreeWidgetItem> parent = items_.contains(parentPath) ? items_[parentPath] : root_;

        QSharedPointer<TreeWidgetItem> newNode(new TreeWidgetItem(nodeType));
        newNode->setText(0, path.last());
        newNode->setIcon(0, QIcon(nodeType == PboNodeType::File ? ":ifile.png" : ":ifolderclosed.png"));

        items_.insert(path, newNode);
        parent->addChildSorted(newNode.get());
    }

    void TreeControl::commitRoot() const {
        tree_->addTopLevelItem(root_.get());
        root_->setExpanded(true);
    }
}
