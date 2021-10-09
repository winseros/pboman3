#pragma once

#include "binarysource.h"

namespace pboman3 {
    struct PboDataInfo {
        qint32 originalSize;
        qint32 dataSize;
        qsizetype dataOffset;
        qint32 timestamp;
        qint32 compressed;
    };

    class PboBinarySource : public BinarySource {
    public:
        PboBinarySource(const QString& path, const PboDataInfo& dataInfo, qsizetype bufferSize = 1024 * 1024);

        void writeToPbo(QFileDevice* targetFile, const Cancel& cancel) override;

        void writeToFs(QFileDevice* targetFile, const Cancel& cancel) override;

        const PboDataInfo& getInfo() const;

        qint32 readOriginalSize() const override;

        qint32 readTimestamp() const override;

        bool isCompressed() const override;

    private:
        PboDataInfo dataInfo_;
        qsizetype bufferSize_;

        void writeRaw(QFileDevice* targetFile, const Cancel& cancel) const;

        bool tryWriteDecompressed(QFileDevice* targetFile, const Cancel& cancel) const;
    };
}
