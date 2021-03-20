#pragma once

#include "binarysource.h"

namespace pboman3 {
    class FsRawBinarySource : public BinarySource {
    public:
        FsRawBinarySource(const QString& path, size_t bufferSize = 1024 ^ 3);

        void writeToPbo(QFileDevice* targetFile, const Cancel& cancel) override;

        void writeToFs(QFileDevice* targetFile, const Cancel& cancel) override;

    private:
        size_t bufferSize_;

        void writeRaw(QFileDevice* targetFile, const Cancel& cancel) const;
    };
}
