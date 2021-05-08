#include "deleteop.h"

namespace pboman3 {
    DeleteOp::DeleteOp(PboModel2* model)
        : model_(model) {
    }

    void DeleteOp::schedule(QList<PboPath> paths) {
        paths_ = std::move(paths);
    }

    void DeleteOp::commit() {
        for (const PboPath& p : paths_)
            model_->removeNode(p);
        reset();
    }

    void DeleteOp::reset() {
        paths_.clear();
    }
}
