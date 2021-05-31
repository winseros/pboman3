#include "treewidgetbase.h"
#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>

namespace pboman3 {
    void TreeWidgetBase::setRoot(PboNode* rootNode) {
        if (root_)
            resetRoot();

        root_ = new TreeWidgetItem(rootNode);
        addTopLevelItem(root_);
        root_->setExpanded(true);
    }

    void TreeWidgetBase::resetRoot() {
        clear();
        root_ = nullptr;
    }

    TreeWidgetBase::TreeWidgetBase(QWidget* parent)
        : QTreeWidget(parent),
          root_(nullptr),
          dragOverItem_(nullptr) {
        connect(this, &QTreeWidget::itemExpanded, this, &TreeWidgetBase::onItemExpanded);
        connect(this, &QTreeWidget::itemCollapsed, this, &TreeWidgetBase::onItemCollapsed);
    }

    QList<PboNode*> TreeWidgetBase::getSelectedHierarchies() const {
        QList<QTreeWidgetItem*> items = selectedItems();
        QSet selected(items.begin(), items.end());

        QList<PboNode*> result;
        result.reserve(items.length());

        for (QTreeWidgetItem* item : selected) {
            QTreeWidgetItem* p = item->parent();
            bool abortItem = false;
            while (p && !abortItem) {
                if (selected.contains(p)) {
                    abortItem = true;
                    continue;
                }
                p = p->parent();
            }
            if (!abortItem)
                result.append(dynamic_cast<TreeWidgetItem*>(item)->node());
        }

        return result;
    }

    PboNode* TreeWidgetBase::getCurrentFolder() const {
        PboNode* result = nullptr;
        auto* selected = dynamic_cast<TreeWidgetItem*>(currentItem());
        if (selected) {
            if (selected->node()->nodeType() == PboNodeType::Container || selected->node()->nodeType() ==
                PboNodeType::Folder) {
                result = selected->node();
            } else {
                result = selected->node()->parentNode();
            }
        }

        return result;
    }

    PboNode* TreeWidgetBase::getCurrentFile() const {
        PboNode* result = nullptr;
        auto* selected = dynamic_cast<TreeWidgetItem*>(currentItem());
        if (selected) {
            if (selected->node()->nodeType() == PboNodeType::File) {
                result = selected->node();
            }
        }

        return result;
    }

    void TreeWidgetBase::onItemExpanded(QTreeWidgetItem* item) {
        item->setIcon(0, QIcon(":ifolderopened.png"));
    }

    void TreeWidgetBase::onItemCollapsed(QTreeWidgetItem* item) {
        item->setIcon(0, QIcon(":ifolderclosed.png"));
    }

    void TreeWidgetBase::startDrag(Qt::DropActions supportedActions) {
        QList<PboNode*> selected = getSelectedHierarchies();

        for (PboNode* item : selected) {
            if (item->nodeType() == PboNodeType::Container)
                selected.removeOne(item);
        }

        if (selected.length()) {
            dragStarted(selected);
        }
    }

    void TreeWidgetBase::dragEnterEvent(QDragEnterEvent* event) {
        if (tryAcceptEvent(event->mimeData(), event->position().toPoint())) {
            event->acceptProposedAction();
        } else {
            event->ignore();
        }
    }

    void TreeWidgetBase::dragLeaveEvent(QDragLeaveEvent* event) {
        setDragOverItem(nullptr);
    }

    void TreeWidgetBase::dragMoveEvent(QDragMoveEvent* event) {
        if (tryAcceptEvent(event->mimeData(), event->position().toPoint())) {
            event->acceptProposedAction();
        } else {
            event->ignore();
        }
    }

    bool TreeWidgetBase::tryAcceptEvent(const QMimeData* mimeData, const QPoint& pos) {
        if (mimeData->hasUrls()) {
            QTreeWidgetItem* treeItem = itemAt(pos);
            if (!treeItem)
                treeItem = topLevelItem(0);
            auto* item = dynamic_cast<TreeWidgetItem*>(treeItem);
            if (item->node()->nodeType() == PboNodeType::File)
                item = dynamic_cast<TreeWidgetItem*>(dynamic_cast<QTreeWidgetItem*>(item)->parent());
            setDragOverItem(item);
            return true;
        }
        setDragOverItem(nullptr);
        return false;
    }

    void TreeWidgetBase::setDragOverItem(TreeWidgetItem* dragOverItem) {
        if (dragOverItem_) {
            dragOverItem_->setBackground(0, QBrush());
            dragOverItem_->setForeground(0, QBrush());
            dragOverItem_ = nullptr;
        }
        if (dragOverItem) {
            dragOverItem->setBackground(0, QBrush(QColor(0, 126, 216)));
            dragOverItem->setForeground(0, Qt::white);
            dragOverItem_ = dragOverItem;
        }
    }

    void TreeWidgetBase::dropEvent(QDropEvent* event) {
        if (event->mimeData()->hasUrls() && dragOverItem_) {
            event->accept();
            dragDropped(dragOverItem_->node(), event->mimeData());
            setDragOverItem(nullptr);
        }

    }
}
