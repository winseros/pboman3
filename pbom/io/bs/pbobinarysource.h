#pragma once

#include "binarysource.h"

namespace pboman3 {
    struct PboDataInfo {
        const int originalSize;
        const int dataSize;
        const size_t dataOffset;
        const bool compressed;

        PboDataInfo(int pOriginalSize, int pDataSize, size_t pDataOffset, bool pCompressed);
    };

    class PboBinarySource : public BinarySource {
    public:
        PboBinarySource(const QString& path, const PboDataInfo& dataInfo, size_t bufferSize = 1024 * 1024);

        void writeToPbo(QFileDevice* targetFile, const Cancel& cancel) override;

        void writeToFs(QFileDevice* targetFile, const Cancel& cancel) override;

        const PboDataInfo& getInfo() const;

        quint32 readOriginalSize() const override;

        quint32 readTimestamp() const override;

        bool isCompressed() const override;

    private:
        PboDataInfo dataInfo_;
        size_t bufferSize_;

        void writeRaw(QFileDevice* targetFile, const Cancel& cancel) const;

        bool tryWriteDecompressed(QFileDevice* targetFile, const Cancel& cancel) const;
    };
}
