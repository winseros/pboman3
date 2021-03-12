#pragma once

#include <QFileDevice>
#include "util/util.h"

namespace pboman3 {
    class BinarySource {
    public:
        BinarySource(const QString& path);

        virtual ~BinarySource();

        virtual void writeDecompressed(QFileDevice* targetFile, const Cancel& cancel) = 0;

        virtual void writeCompressed(QFileDevice* targetFile, const Cancel& cancel) = 0;

        virtual void writeRaw(QFileDevice* targetFile, const Cancel& cancel) = 0;

        const QString& path() const;

    protected:
        QFileDevice* file_;

    private:
        QString path_;
    };

    class FileBasedBinarySource : public BinarySource {
    public:
        FileBasedBinarySource(const QString& path, size_t bufferSize = 1024 ^ 3);

        void writeDecompressed(QFileDevice* targetFile, const Cancel& cancel) override;

        void writeCompressed(QFileDevice* targetFile, const Cancel& cancel) override;

        void writeRaw(QFileDevice* targetFile, const Cancel& cancel) override;
    private:
        size_t bufferSize_;
    };

    struct PboDataInfo {
        const int originalSize;
        const int dataSize;
        const size_t dataOffset;

        PboDataInfo(int pOriginalSize, int pDataSize, size_t pDataOffset);
    };

    class PboBasedBinarySource : public BinarySource {
    public:
        PboBasedBinarySource(const QString& path, const PboDataInfo& dataInfo, size_t bufferSize = 1024 ^ 3);

        void writeDecompressed(QFileDevice* targetFile, const Cancel& cancel) override;

        void writeCompressed(QFileDevice* targetFile, const Cancel& cancel) override;

        void writeRaw(QFileDevice* targetFile, const Cancel& cancel) override;

        const PboDataInfo& getInfo() const;

    private:
        PboDataInfo dataInfo_;
        size_t bufferSize_;

        bool tryWriteDecompressed(QFileDevice* targetFile, const Cancel& cancel) const;

        bool isCompressed() const;
    };
}
