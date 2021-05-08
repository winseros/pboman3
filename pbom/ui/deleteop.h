#pragma once
#include "model/pbomodel2.h"
#include "model/pbopath.h"

namespace pboman3 {
    class DeleteOp {
    public:
        DeleteOp(PboModel2* model);

        void schedule(QList<PboPath> paths);

        void commit();

        void reset();

    private:
        PboModel2* model_;
        QList<PboPath> paths_;
    };
}
