#include "pbobinarysource.h"
#include "io/pboioexception.h"
#include "io/lzh/lzh.h"
#include "io/lzh/lzhdecompressionexception.h"

namespace pboman3 {
    PboBinarySource::PboBinarySource(const QString& path, const PboDataInfo& dataInfo, size_t bufferSize)
        : BinarySource(path),
          dataInfo_(dataInfo),
          bufferSize_(bufferSize) {
    }

    void PboBinarySource::writeToPbo(QFileDevice* targetFile, const Cancel& cancel) {
        assert(file_->isOpen());
        writeRaw(targetFile, cancel);
    }

    void PboBinarySource::writeToFs(QFileDevice* targetFile, const Cancel& cancel) {
        assert(file_->isOpen());
        if (isCompressed()) {
            if (!tryWriteDecompressed(targetFile, cancel))
                writeRaw(targetFile, cancel);
        } else {
            writeRaw(targetFile, cancel);
        }
    }

    void PboBinarySource::writeRaw(QFileDevice* targetFile, const Cancel& cancel) const {
        file_->seek(dataInfo_.dataOffset);

        QByteArray buf(bufferSize_, Qt::Initialization::Uninitialized);

        int remaining = dataInfo_.dataSize;
        while (!cancel() && remaining > 0) {
            const qsizetype willRead = remaining > buf.size() ? buf.size() : remaining;
            const quint64 hasRead = file_->read(buf.data(), willRead);
            if (hasRead <= 0)
                throw PboIoException("For some reason could not read from the file.", file_->fileName());
            targetFile->write(buf.data(), hasRead);
            remaining -= hasRead;
        }
    }

    bool PboBinarySource::tryWriteDecompressed(QFileDevice* targetFile, const Cancel& cancel) const {
        try {
            file_->seek(dataInfo_.dataOffset);
            Lzh::decompress(file_, targetFile, dataInfo_.originalSize, cancel);
            return true;
        } catch (LzhDecompressionException&) {
            targetFile->resize(0);
            return false;
        }
    }

    const PboDataInfo& PboBinarySource::getInfo() const {
        return dataInfo_;
    }

    quint32 PboBinarySource::readOriginalSize() const {
        return dataInfo_.originalSize;
    }

    quint32 PboBinarySource::readTimestamp() const {
        return dataInfo_.timestamp;
    }

    bool PboBinarySource::isCompressed() const {
        return dataInfo_.compressed;
    }
}
