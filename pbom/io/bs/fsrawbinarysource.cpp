#include "fsrawbinarysource.h"
#include <QFileInfo>

namespace pboman3::io {
    FsRawBinarySource::FsRawBinarySource(QString path, qsizetype bufferSize)
        : BinarySourceBase(std::move(path)),
          bufferSize_(bufferSize) {
    }

    void FsRawBinarySource::writeToPbo(QFileDevice* targetFile, const Cancel& cancel) {
        assert(file_->isOpen());
        writeRaw(targetFile, cancel);
    }

    void FsRawBinarySource::writeToFs(QFileDevice* targetFile, const Cancel& cancel) {
        assert(file_->isOpen());
        writeRaw(targetFile, cancel);
    }

    void FsRawBinarySource::writeRaw(QFileDevice* targetFile, const Cancel& cancel) const {
        const bool seek = file_->seek(0);
        assert(seek);

        QByteArray buf(bufferSize_, Qt::Initialization::Uninitialized);

        qsizetype remaining = file_->size();
        while (!cancel() && remaining > 0) {
            const qsizetype willRead = remaining > buf.size() ? buf.size() : remaining;
            const qint64 hasRead = file_->read(buf.data(), willRead);
            targetFile->write(buf.data(), hasRead);
            remaining -= hasRead;
        }
    }

    qint32 FsRawBinarySource::readOriginalSize() const {
        const QFileInfo fi(path());
        return static_cast<qint32>(fi.size());
    }

    qint32 FsRawBinarySource::readTimestamp() const {
        const QFileInfo fi(path());
        return static_cast<qint32>(fi.lastModified().toSecsSinceEpoch());
    }

    bool FsRawBinarySource::isCompressed() const {
        return false;
    }
}
