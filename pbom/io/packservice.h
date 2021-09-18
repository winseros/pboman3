#pragma once

#include "pbofile.h"
#include <QDir>
#include <QPromise>
#include "util/util.h"

namespace pboman3 {
    class PackService {
    public:
        void unpack(QPromise<void> promise, PboFile& file, const QDir& targetDir, const Cancel& cancel) const;
    };
}
