#include "treewidget.h"
#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>

namespace pboman3 {
    TreeWidget::TreeWidget(QWidget* parent)
        : QTreeWidget(parent),
          dragOverItem_(nullptr) {
        root_ = QSharedPointer<TreeWidgetItem>(new TreeWidgetItem(PboNodeType::Container));
        connect(this, &QTreeWidget::itemExpanded, this, &TreeWidget::onItemExpanded);
        connect(this, &QTreeWidget::itemCollapsed, this, &TreeWidget::onItemCollapsed);
    }

    void TreeWidget::setNewRoot(const QString& fileName) {
        clear();
        root_->setText(0, fileName);
        root_->setIcon(0, QIcon(":ifolderopened.png"));
    }

    void TreeWidget::addNewNode(const PboPath& path, PboNodeType nodeType) const {
        const PboPath parentPath = path.makeParent();
        TreeWidgetItem* parent = parentPath.length() ? root_->get(parentPath) : root_.get();
        if (!parent) parent = root_.get();

        auto* newNode = new TreeWidgetItem(nodeType);
        newNode->setText(0, path.last());
        newNode->setIcon(0, QIcon(nodeType == PboNodeType::File ? ":ifile.png" : ":ifolderclosed.png"));

        parent->addChildSorted(newNode);
    }

    void TreeWidget::removeNode(const PboPath& path) const {
        TreeWidgetItem* node = root_->get(path);
        QTreeWidgetItem* parent = node->parent();
        parent->removeChild(node);
        delete node;
    }

    void TreeWidget::commitRoot() {
        addTopLevelItem(root_.get());
        root_->setExpanded(true);
    }

    bool TreeWidget::isSelectionValid() const {
        return !topLevelItem(0)->isSelected();
    }

    QList<TreeWidgetItem*> TreeWidget::getSelectedItems() const {
        const QList<QTreeWidgetItem*> selected = selectedItems();
        QList<TreeWidgetItem*> result;
        result.reserve(selected.length());

        for (QTreeWidgetItem* item: selected) {
            QTreeWidgetItem* p = item->parent();
            bool abortItem = false;
            while (p && !abortItem) {
                if (p->isSelected()) {
                    abortItem = true;
                    continue;
                }
                p = p->parent();
            }
            if (!abortItem)
                result.append(dynamic_cast<TreeWidgetItem*>(item));
        }

        return result;
    }

    TreeWidgetItem* TreeWidget::getSelectedFolder() const {
        TreeWidgetItem* result = nullptr;
        const QList<QTreeWidgetItem*> selected = selectedItems();
        if (selected.length() == 1) {
            auto* item = dynamic_cast<TreeWidgetItem*>(selected.at(0));
            if (item->nodeType() == PboNodeType::Container || item->nodeType() == PboNodeType::Folder) {
                result = item;
            }
        }
        return result;
    }

    TreeWidgetItem* TreeWidget::getSelectedFile() const {
        TreeWidgetItem* result = nullptr;
        const QList<QTreeWidgetItem*> selected = selectedItems();
        if (selected.length() == 1) {
            auto* item = dynamic_cast<TreeWidgetItem*>(selected.at(0));
            if (item->nodeType() == PboNodeType::File) {
                result = item;
            }
        }
        return result;
    }

    QList<PboPath> TreeWidget::getSelectedPaths() const {
        const QList<TreeWidgetItem*> selected = getSelectedItems();

        QList<PboPath> paths;
        paths.reserve(selected.length());
        for (const TreeWidgetItem* item : selected)
            paths.append(item->makePath());

        return paths;
    }

    void TreeWidget::onItemExpanded(QTreeWidgetItem* item) {
        item->setIcon(0, QIcon(":ifolderopened.png"));
    }

    void TreeWidget::onItemCollapsed(QTreeWidgetItem* item) {
        item->setIcon(0, QIcon(":ifolderclosed.png"));
    }

    void TreeWidget::startDrag(Qt::DropActions supportedActions) {
        QList<PboPath> paths;
        for (const QTreeWidgetItem* selected : selectedItems()) {
            const auto* sel = dynamic_cast<const TreeWidgetItem*>(selected);
            assert(sel && "Can not be null");

            if (sel->nodeType() == PboNodeType::File || sel->nodeType() == PboNodeType::Folder) {
                paths.append(sel->makePath());
            }
        }

        if (paths.length()) {
            emit dragStarted(paths);
        }
    }

    void TreeWidget::dragEnterEvent(QDragEnterEvent* event) {
        if (tryAcceptEvent(event->mimeData(), event->position().toPoint())) {
            event->acceptProposedAction();
        } else {
            event->ignore();
        }
    }

    void TreeWidget::dragLeaveEvent(QDragLeaveEvent* event) {
        setDragOverItem(nullptr);
    }

    void TreeWidget::dragMoveEvent(QDragMoveEvent* event) {
        if (tryAcceptEvent(event->mimeData(), event->position().toPoint())) {
            event->acceptProposedAction();
        } else {
            event->ignore();
        }
    }

    bool TreeWidget::tryAcceptEvent(const QMimeData* mimeData, const QPoint& pos) {
        if (mimeData->hasUrls()) {
            auto* item = dynamic_cast<TreeWidgetItem*>(itemAt(pos));
            if (item) {
                if (item->nodeType() == PboNodeType::File)
                    item = dynamic_cast<TreeWidgetItem*>(item->parent());
                setDragOverItem(item);
                return true;
            }
        }
        setDragOverItem(nullptr);
        return false;
    }

    void TreeWidget::setDragOverItem(TreeWidgetItem* dragOverItem) {
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

    void TreeWidget::dropEvent(QDropEvent* event) {
        if (event->mimeData()->hasUrls() && dragOverItem_) {
            event->accept();
            emit dragDropped(dragOverItem_->makePath(), event->mimeData());
            setDragOverItem(nullptr);
        }

    }
}
