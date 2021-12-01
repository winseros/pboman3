#include "deleteop.h"
#include "util/log.h"

#define LOG(...) LOGGER("ui/treewidget/DeleteOp", __VA_ARGS__)

namespace pboman3::ui {
    void DeleteOp::schedule(QList<PboNode*> nodes) {
        LOG(info, "Schedule delete for", nodes.count(), "nodes")
        nodes_ = std::move(nodes);
    }

    void DeleteOp::commit() {
        LOG(info, "Commit delete")
        for (PboNode* p : nodes_) {
            LOG(debug, "Delete the node:", *p)
            p->removeFromHierarchy();
        }
        reset();
    }

    void DeleteOp::reset() {
        LOG(info, "Reset the delete op")
        nodes_.clear();
    }
}
