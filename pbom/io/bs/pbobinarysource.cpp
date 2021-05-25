#include "pbobinarysource.h"
#include "io/lzh/lzh.h"
#include "io/lzh/lzhdecompressionexception.h"

namespace pboman3 {
    PboDataInfo::PboDataInfo(int pOriginalSize, int pDataSize, size_t pDataOffset, bool pCompressed)
        : originalSize(pOriginalSize),
          dataSize(pDataSize),
          dataOffset(pDataOffset),
          compressed(pCompressed) {
    }


    PboBinarySource::PboBinarySource(const QString& path, const PboDataInfo& dataInfo, size_t bufferSize)
        : BinarySource(path),
          dataInfo_(dataInfo),
          bufferSize_(bufferSize) {
    }

    void PboBinarySource::writeToPbo(QFileDevice* targetFile, const Cancel& cancel) {
        writeRaw(targetFile, cancel);
    }

    void PboBinarySource::writeToFs(QFileDevice* targetFile, const Cancel& cancel) {
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
            assert(hasRead > 0 && "For some reason did not read from the file");
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
        return 0;
    }

    bool PboBinarySource::isCompressed() const {
        return dataInfo_.compressed;
    }
}
