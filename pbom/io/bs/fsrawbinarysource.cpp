#include "fsrawbinarysource.h"

namespace pboman3 {
    FsRawBinarySource::FsRawBinarySource(const QString& path, size_t bufferSize)
        : BinarySource(path),
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
}