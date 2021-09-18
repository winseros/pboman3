#pragma once

#include "binarysource.h"

namespace pboman3 {
    class FsRawBinarySource : public BinarySource {
    public:
        FsRawBinarySource(QString path, size_t bufferSize = 1024 * 1024);

        void writeToPbo(QFileDevice* targetFile, const Cancel& cancel) override;

        void writeToFs(QFileDevice* targetFile, const Cancel& cancel) override;

        qint32 readOriginalSize() const override;

        qint32 readTimestamp() const override;

        bool isCompressed() const override;

    private:
        size_t bufferSize_;

        void writeRaw(QFileDevice* targetFile, const Cancel& cancel) const;
    };
}
