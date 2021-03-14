#include "pbobasedbinarysource.h"
#include "io/lzh/lzh.h"
#include "io/lzh/lzhdecompressionexception.h"

namespace pboman3 {
    PboDataInfo::PboDataInfo(int pOriginalSize, int pDataSize, size_t pDataOffset)
        : originalSize(pOriginalSize),
        dataSize(pDataSize),
        dataOffset(pDataOffset) {
    }


    PboBasedBinarySource::PboBasedBinarySource(const QString& path, const PboDataInfo& dataInfo, size_t bufferSize)
        : BinarySource(path),
        dataInfo_(dataInfo),
        bufferSize_(bufferSize) {
    }

    void PboBasedBinarySource::writeDecompressed(QFileDevice* targetFile, const Cancel& cancel) {
        if (isCompressed()) {
            if (!tryWriteDecompressed(targetFile, cancel))
                writeRaw(targetFile, cancel);
        }
        else {
            writeRaw(targetFile, cancel);
        }
    }

    void PboBasedBinarySource::writeCompressed(QFileDevice* targetFile, const Cancel& cancel) {
        if (isCompressed()) {
            writeRaw(targetFile, cancel);
        } else {
            file_->seek(0);
            Lzh::compress(file_, targetFile, cancel);
        }
    }

    bool PboBasedBinarySource::tryWriteDecompressed(QFileDevice* targetFile, const Cancel& cancel) const {
        try {
            file_->seek(dataInfo_.dataOffset);
            Lzh::decompress(file_, targetFile, dataInfo_.originalSize, cancel);
            return true;
        }
        catch (LzhDecompressionException&) {
            targetFile->resize(0);
            return false;
        }
    }

    void PboBasedBinarySource::writeRaw(QFileDevice* targetFile, const Cancel& cancel) {
        file_->seek(dataInfo_.dataOffset);

        QByteArray buf(bufferSize_, Qt::Initialization::Uninitialized);

        int remaining = dataInfo_.dataSize;
        while (!cancel() && remaining > 0) {
            const qsizetype willRead = remaining > buf.size() ? buf.size() : remaining;
            const quint64 hasRead = file_->read(buf.data(), willRead);
            assert(hasRead > 0 && "For some reason did not read from the file");
            targetFile->write(buf.data(), hasRead);
            remaining -= hasRead;
        }
    }

    const PboDataInfo& PboBasedBinarySource::getInfo() const {
        return dataInfo_;
    }

    bool PboBasedBinarySource::isCompressed() const {
        return dataInfo_.originalSize > dataInfo_.dataSize;
    }
}
