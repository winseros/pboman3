#include "treewidgetitem.h"

namespace pboman3 {
    TreeWidgetItem::TreeWidgetItem(PboNodeType nodeType)
        : nodeType_(nodeType) {
    }

    void TreeWidgetItem::addChildSorted(TreeWidgetItem* node) {
        auto index = -1;
        
        for (auto i = 0; i < childCount(); i++) {
            if (*node < *dynamic_cast<TreeWidgetItem*>(child(i))) {
                index = i;
                break;
            }
        }

        if (index < 0)
            addChild(node);
        else
            insertChild(index, node);
    }

    bool TreeWidgetItem::operator<(TreeWidgetItem& other) const {
        if (nodeType_ == other.nodeType_) {
            return QTreeWidgetItem::operator<(other);
        }
        return nodeType_ > other.nodeType_;
    }
}
