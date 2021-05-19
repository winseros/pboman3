#include "fsrawbinarysource.h"
#include <QFileInfo>

namespace pboman3 {
    FsRawBinarySource::FsRawBinarySource(QString path, size_t bufferSize)
        : BinarySource(std::move(path)),
          bufferSize_(bufferSize) {
    }

    void FsRawBinarySource::writeToPbo(QFileDevice* targetFile, const Cancel& cancel) {
        writeRaw(targetFile, cancel);
    }

    void FsRawBinarySource::writeToFs(QFileDevice* targetFile, const Cancel& cancel) {
        writeRaw(targetFile, cancel);
    }

    void FsRawBinarySource::writeRaw(QFileDevice* targetFile, const Cancel& cancel) const {
        file_->seek(0);

        QByteArray buf(bufferSize_, Qt::Initialization::Uninitialized);

        qsizetype remaining = file_->size();
        while (!cancel() && remaining > 0) {
            const qsizetype willRead = remaining > buf.size() ? buf.size() : remaining;
            const quint64 hasRead = file_->read(buf.data(), willRead);
            targetFile->write(buf.data(), hasRead);
            remaining -= hasRead;
        }
    }

    quint32 FsRawBinarySource::readOriginalSize() const {
        const QFileInfo fi(path());
        return static_cast<quint32>(fi.size());
    }

    quint32 FsRawBinarySource::readTimestamp() const {
        const QFileInfo fi(path());
        return static_cast<quint32>(fi.lastModified().toSecsSinceEpoch());
    }

    bool FsRawBinarySource::isCompressed() const {
        return false;
    }
}
