#pragma once

#include <QFileDevice>
#include "util/util.h"

namespace pboman3 {
    class BinarySource {
    public:
        BinarySource(const QString& path);

        virtual ~BinarySource();

        virtual void writeTo(QFileDevice* targetFile, const Cancel& cancel) = 0;

    protected:
        QFileDevice* file_;
    };

    class FileBasedBinarySource : public BinarySource {
    public:
        FileBasedBinarySource(const QString& path, size_t bufferSize = 1024 ^ 3);

        void writeTo(QFileDevice* targetFile, const Cancel& cancel) override;

    private:
        size_t bufferSize_;
    };

    struct PboDataInfo {
        const int dataSize;
        const size_t dataOffset;
    };

    class PboBasedBinarySource : public BinarySource {
    public:
        PboBasedBinarySource(const QString& path, const PboDataInfo& dataInfo, size_t bufferSize = 1024 ^ 3);

        void writeTo(QFileDevice* targetFile, const Cancel& cancel) override;

    private:
        PboDataInfo dataInfo_;
        size_t bufferSize_;
    };
}
