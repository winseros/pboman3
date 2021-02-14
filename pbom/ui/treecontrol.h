#pragma once

#include "treewidgetitem.h"
#include "model/pbonodetype.h"
#include "model/pbopath.h"

namespace pboman3 {
    class TreeControl {
    public:
        explicit TreeControl(QTreeWidget* tree);

        void setNewRoot(const QString& fileName) const;

        void addNewNode(const PboPath& path, PboNodeType nodeType);

        void commitRoot() const;
    private:
        QTreeWidget* tree_;
        QSharedPointer<TreeWidgetItem> root_;
        QMap<PboPath, QSharedPointer<TreeWidgetItem>> items_;
    };
}
