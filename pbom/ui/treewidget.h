#pragma once

#include <QTreeWidget>
#include "treewidgetitem.h"
#include "model/pbonodetype.h"
#include "model/pbopath.h"

namespace pboman3 {
    class TreeWidget : public QTreeWidget {
    Q_OBJECT
    public:
        explicit TreeWidget(QWidget* parent = nullptr);

        void setNewRoot(const QString& fileName);

        void addNewNode(const PboPath& path, PboNodeType nodeType) const;

        void removeNode(const PboPath& path) const;

        void commitRoot();

        bool isSelectionValid() const;

        QList<TreeWidgetItem*> getSelectedItems() const;

        TreeWidgetItem* getSelectedFolder() const;

        TreeWidgetItem* getSelectedFile() const;

        QList<PboPath> getSelectedPaths() const;

    signals:
        void dragStarted(const QList<PboPath>& items);

        void dragDropped(const PboPath& target, const QMimeData* mimeData);

    protected:
        void startDrag(Qt::DropActions supportedActions) override;

        void dragEnterEvent(QDragEnterEvent* event) override;

        void dragLeaveEvent(QDragLeaveEvent* event) override;

        void dragMoveEvent(QDragMoveEvent* event) override;

        void dropEvent(QDropEvent* event) override;

    private:
        QSharedPointer<TreeWidgetItem> root_;
        TreeWidgetItem* dragOverItem_;

        void onItemExpanded(QTreeWidgetItem* item);

        void onItemCollapsed(QTreeWidgetItem* item);

        bool tryAcceptEvent(const QMimeData* mimeData, const QPoint& pos);

        void setDragOverItem(TreeWidgetItem* dragOverItem);
    };
}
