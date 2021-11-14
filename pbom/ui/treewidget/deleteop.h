#pragma once

#include "domain/pbonode.h"

namespace pboman3 {
    using namespace domain;

    class DeleteOp {
    public:
        void schedule(QList<PboNode*> nodes);

        void commit();

        void reset();

    private:
        QList<PboNode*> nodes_;
    };
}
