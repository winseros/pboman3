#pragma once

#include "fsrawbinarysource.h"

namespace pboman3 {
    class FsLzhBinarySource: public FsRawBinarySource {
    public:
        FsLzhBinarySource(const QString& path, size_t bufferSize = 1024 ^ 3);

        void writeToPbo(QFileDevice* targetFile, const Cancel& cancel) override;
    };
}
