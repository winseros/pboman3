#include "deleteop.h"

namespace pboman3 {
    void DeleteOp::schedule(QList<PboNode*> nodes) {
        nodes_ = std::move(nodes);
    }

    void DeleteOp::commit() {
        for (PboNode* p : nodes_)
            p->removeFromHierarchy();
        reset();
    }

    void DeleteOp::reset() {
        nodes_.clear();
    }
}
