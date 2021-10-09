#pragma once

#include "fsrawbinarysource.h"

namespace pboman3 {
    class FsLzhBinarySource: public FsRawBinarySource {
    public:
        FsLzhBinarySource(QString path, qsizetype bufferSize = 1024 * 1024);

        void writeToPbo(QFileDevice* targetFile, const Cancel& cancel) override;

        bool isCompressed() const override;
    };
}
