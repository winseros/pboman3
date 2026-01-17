#include "treewidgetbase.h"
#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>
#include "exception.h"
#include "util/log.h"

#define LOG(...) LOGGER("ui/treewidget/TreeWidgetBase", __VA_ARGS__)

namespace pboman3::ui {
    TreeWidgetBase::TreeWidgetBase(QWidget* parent)
        : QTreeWidget(parent),
          root_(nullptr),
          dragOverItem_(nullptr) {
        connect(this, &QTreeWidget::itemExpanded, this, &TreeWidgetBase::itemSyncIcon);
        connect(this, &QTreeWidget::itemCollapsed, this, &TreeWidgetBase::itemSyncIcon);
    }

    void TreeWidgetBase::setRoot(PboNode* rootNode) {
        if (root_)
            resetRoot();

        LOG(info, "Set new root node:", *rootNode)

        root_ = new TreeWidgetItem(rootNode);
        addTopLevelItem(root_);
        root_->setExpanded(true);
    }

    void TreeWidgetBase::resetRoot() {
        LOG(info, "Reset the root node")
        clear();
        root_ = nullptr;
    }

    QList<PboNode*> TreeWidgetBase::getSelectedHierarchies() const {
        QList<QTreeWidgetItem*> items = selectedItems();
        QList<PboNode*> result;

        if (items.count() > 1) {
            result.reserve(items.length());

            for (QTreeWidgetItem* item : items) {
                QTreeWidgetItem* p = item->parent();
                bool abortItem = false;
                while (p && !abortItem) {
                    if (items.contains(p))
                        abortItem = true;
                    else
                        p = p->parent();
                }
                if (!abortItem)
                    result.append(dynamic_cast<TreeWidgetItem*>(item)->node());
            }
        } else if (items.count() == 1) {
            for (QTreeWidgetItem* item : items) {
                result.append(dynamic_cast<TreeWidgetItem*>(item)->node());
            }
        }

        return result;
    }

    PboNode* TreeWidgetBase::getSelectionRoot() const {
        QList<QTreeWidgetItem*> items = selectedItems();

        //more than 1 item selected
        if (items.count() > 1) {
            PboNode* p;
            QHash<PboNode*, int> counts;
            auto last = items.end();
            --last;
            auto it = items.begin();
            while (it != last) {
                p = dynamic_cast<TreeWidgetItem*>(*it)->node();
                while (p) {
                    counts[p] = counts.contains(p) ? counts[p] + 1 : 1;
                    p = p->parentNode();
                }
                ++it;
            }
            p = dynamic_cast<TreeWidgetItem*>(*last)->node();
            while (p) {
                counts[p] = counts.contains(p) ? counts[p] + 1 : 1;
                if (counts[p] == items.count())
                    return p; //this is the normal return point of the code
                p = p->parentNode();
            }
            throw AppException("The code must have never reached this line");
        }

        //1 item selected
        if (items.count() > 0) {
            const auto* item = dynamic_cast<TreeWidgetItem*>(items.at(0));
            return item->node();
        }

        //no items selected
        return nullptr;
    }

    PboNode* TreeWidgetBase::getCurrentFolder() const {
        PboNode* result = nullptr;

        const QList<QTreeWidgetItem*> selected = selectedItems();
        if (selected.length() == 1) {
            const auto selectedItem = dynamic_cast<TreeWidgetItem*>(selected.at(0));
            if (selectedItem->node()->nodeType() == PboNodeType::Container
                || selectedItem->node()->nodeType() == PboNodeType::Folder) {
                result = selectedItem->node();
            }
        }

        return result;
    }

    PboNode* TreeWidgetBase::getCurrentFile() const {
        PboNode* result = nullptr;

        const QList<QTreeWidgetItem*> selected = selectedItems();
        if (selected.length() == 1) {
            const auto selectedItem = dynamic_cast<TreeWidgetItem*>(selected.at(0));
            if (selectedItem->node()->nodeType() == PboNodeType::File) {
                result = selectedItem->node();
            }
        }

        return result;
    }

    void TreeWidgetBase::itemSyncIcon(QTreeWidgetItem* item) {
        auto* treeWidgetItem = dynamic_cast<TreeWidgetItem*>(item);
        assert(treeWidgetItem);
        treeWidgetItem->folderSyncIcon();
    }

    void TreeWidgetBase::startDrag(Qt::DropActions supportedActions) {
        QList<PboNode*> selected = getSelectedHierarchies();

        for (PboNode* item : selected) {
            if (item->nodeType() == PboNodeType::Container)
                selected.removeOne(item);
        }

        if (!selected.empty()) {
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
