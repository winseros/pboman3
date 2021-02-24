#pragma once

#include <QTreeWidgetItem>
#include "model/pbonodetype.h"
#include "model/pbopath.h"

namespace pboman3 {
    class TreeWidgetItem : public QTreeWidgetItem {
    public:
        TreeWidgetItem(PboNodeType nodeType);

        void addChildSorted(TreeWidgetItem* node);

        bool operator <(TreeWidgetItem& other) const;

        PboNodeType nodeType() const;

        PboPath makePath() const;

        TreeWidgetItem* get(const PboPath& node) const;
    private:
        const PboNodeType nodeType_;

        TreeWidgetItem* findChild(const QString& title) const;
    };
}
