#pragma once

#include "model/pbomodel.h"

namespace pboman3 {
    class DeleteOp {
    public:
        void schedule(QList<PboNode*> nodes);

        void commit();

        void reset();

    private:
        QList<PboNode*> nodes_;
    };
}
