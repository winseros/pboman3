#pragma once

#include <QTreeWidget>
#include "treewidgetitem.h"
#include "domain/pbonode.h"

namespace pboman3 {
    using namespace domain;

    class TreeWidgetBase : public QTreeWidget {
    Q_OBJECT
    public:
        void setRoot(PboNode* rootNode);

        void resetRoot();

        QList<PboNode*> getSelectedHierarchies() const;

        PboNode* getSelectionRoot() const;

    protected:
        explicit TreeWidgetBase(QWidget* parent = nullptr);

        PboNode* getCurrentFolder() const;

        PboNode* getCurrentFile() const;

        virtual void dragStarted(const QList<PboNode*>& items) = 0;

        virtual void dragDropped(PboNode* target, const QMimeData* mimeData) = 0;

        void startDrag(Qt::DropActions supportedActions) override;

        void dragEnterEvent(QDragEnterEvent* event) override;

        void dragLeaveEvent(QDragLeaveEvent* event) override;

        void dragMoveEvent(QDragMoveEvent* event) override;

        void dropEvent(QDropEvent* event) override;

    private:
        TreeWidgetItem* root_;
        TreeWidgetItem* dragOverItem_;

        void itemSyncIcon(QTreeWidgetItem* item);

        bool tryAcceptEvent(const QMimeData* mimeData, const QPoint& pos);

        void setDragOverItem(TreeWidgetItem* dragOverItem);
    };
}
