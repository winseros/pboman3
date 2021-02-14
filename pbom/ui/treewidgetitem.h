#pragma once

#include <QTreeWidgetItem>
#include "model/pbonodetype.h"

namespace pboman3 {
    class TreeWidgetItem : public QTreeWidgetItem {
    public:
        TreeWidgetItem(PboNodeType nodeType);

        void addChildSorted(TreeWidgetItem* node);

        bool operator <(TreeWidgetItem& other) const;

    private:
        const PboNodeType nodeType_;
    };
}
