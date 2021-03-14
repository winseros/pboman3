#pragma once

#include "binarysource.h"

namespace pboman3 {
    class FileBasedBinarySource : public BinarySource {
    public:
        FileBasedBinarySource(const QString& path, size_t bufferSize = 1024 ^ 3);

        void writeDecompressed(QFileDevice* targetFile, const Cancel& cancel) override;

        void writeCompressed(QFileDevice* targetFile, const Cancel& cancel) override;

        void writeRaw(QFileDevice* targetFile, const Cancel& cancel) override;
    private:
        size_t bufferSize_;
    };
}
