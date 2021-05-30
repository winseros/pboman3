#include "treewidgetitem.h"
#include "ui/renamedialog.h"

namespace pboman3 {
    TreeWidgetItem::TreeWidgetItem(PboNode* node)
        : QTreeWidgetItem(), QObject(), node_(node) {

        setText(0, node_->title());
        setIcon(0, QIcon(node->nodeType() == PboNodeType::File ? ":ifile.png" : ":ifolderclosed.png"));

        initCreateChildren();

        connect(node_, &PboNode::titleChanged, this, &TreeWidgetItem::onNodeTitleChanged);
        connect(node_, &PboNode::childCreated, this, &TreeWidgetItem::onNodeChildCreated);
        connect(node_, &PboNode::childMoved, this, &TreeWidgetItem::onNodeChildMoved);
        connect(node_, &PboNode::childRemoved, this, &TreeWidgetItem::onNodeChildRemoved);
    }

    PboNode* TreeWidgetItem::node() const {
        return node_;
    }

    void TreeWidgetItem::rename() const {
        auto validate = [this](const QString& title) {
            return node_->verifyTitle(title);
        };

        RenameDialog rename(treeWidget(), node_->title(), validate);
        if (rename.exec() == QDialog::DialogCode::Accepted) {
            node_->setTitle(rename.title());
        }
    }

    void TreeWidgetItem::initCreateChildren() {
        qsizetype i = 0;
        for (const QSharedPointer<PboNode>& child : *node_) {
            onNodeChildCreated(child.get(), i);
            i++;
        }
    }

    void TreeWidgetItem::onNodeTitleChanged(const QString& title) {
        setText(0, title);
    }

    void TreeWidgetItem::onNodeChildCreated(PboNode* node, qsizetype index) {
        auto* item = new TreeWidgetItem(node);
        insertChild(static_cast<int>(index), item);
    }

    void TreeWidgetItem::onNodeChildMoved(qsizetype prevIndex, qsizetype newIndex) {
        QTreeWidgetItem* item = takeChild(static_cast<int>(prevIndex));
        insertChild(static_cast<int>(newIndex), item);
        treeWidget()->setCurrentItem(item);
    }

    void TreeWidgetItem::onNodeChildRemoved(qsizetype index) {
        QTreeWidgetItem* item = child(static_cast<int>(index));
        removeChild(item);
        delete item;
    }
}
