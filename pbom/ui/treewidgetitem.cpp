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


    PboNodeType TreeWidgetItem::nodeType() const {
        return nodeType_;
    }

    PboPath TreeWidgetItem::makePath() const {
        PboPath p;
        auto* parent = const_cast<TreeWidgetItem*>(this);
        while (parent->nodeType_ != PboNodeType::Container) {
            p.prepend(parent->text(0));
            parent = dynamic_cast<TreeWidgetItem*>(parent->parent());
        }
        return p;
    }

    TreeWidgetItem* TreeWidgetItem::get(const PboPath& node) const {
        TreeWidgetItem* result = findChild(node.first());
        if (!result)
            return nullptr;

        auto it = node.begin();
        ++it;

        while (it != node.end()) {
            result = result->findChild(*it);
            if (!result)
                return nullptr;
            ++it;
        }
        return result;
    }

    TreeWidgetItem* TreeWidgetItem::findChild(const QString& title) const {
        for (int i = 0; i < this->childCount(); i++) {
            if (child(i)->text(0) == title)
                return dynamic_cast<TreeWidgetItem*>(child(i));
        }
        return nullptr;
    }
}
