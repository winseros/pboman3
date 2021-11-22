#include "treewidgetitem.h"
#include <QFileInfo>
#include "ui/renamedialog.h"
#include "ui/win32/win32iconmgr.h"

namespace pboman3::ui {
    TreeWidgetItem::TreeWidgetItem(PboNode* node)
        : TreeWidgetItem(node, QSharedPointer<IconMgr>(new Win32IconMgr)) {
    }

    TreeWidgetItem::TreeWidgetItem(PboNode* node, const QSharedPointer<IconMgr>& icons)
        : QTreeWidgetItem(),
          QObject(),
          node_(node),
          icons_(icons) {
        setText(0, node_->title());
        setIcon(0, node->nodeType() == PboNodeType::File
                       ? icons_->getIconForExtension(GetFileExtension(node_->title()))
                       : icons_->getFolderClosedIcon());

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
        RenameDialog(treeWidget(), node_).exec();
    }

    void TreeWidgetItem::folderSyncIcon() {
        if (node_->nodeType() != PboNodeType::File) {
            setIcon(0, isExpanded()
                           ? icons_->getFolderOpenedIcon()
                           : icons_->getFolderClosedIcon());
        }
    }

    void TreeWidgetItem::initCreateChildren() {
        qsizetype i = 0;
        for (PboNode* child : *node_) {
            onNodeChildCreated(child, i);
            i++;
        }
    }

    void TreeWidgetItem::onNodeTitleChanged(const QString& title) {
        setText(0, title);
        if (node_->nodeType() == PboNodeType::File) {
            setIcon(0, icons_->getIconForExtension(GetFileExtension(node_->title())));
        }
    }

    void TreeWidgetItem::onNodeChildCreated(PboNode* node, qsizetype index) {
        auto* item = new TreeWidgetItem(node, icons_);
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
