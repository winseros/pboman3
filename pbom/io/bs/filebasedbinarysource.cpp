#include "filebasedbinarysource.h"
#include "io/lzh/lzh.h"

namespace pboman3 {
    FileBasedBinarySource::FileBasedBinarySource(const QString& path, size_t bufferSize)
        : BinarySource(path),
        bufferSize_(bufferSize) {
    }

    void FileBasedBinarySource::writeDecompressed(QFileDevice* targetFile, const Cancel& cancel) {
        writeRaw(targetFile, cancel);
    }

    void FileBasedBinarySource::writeCompressed(QFileDevice* targetFile, const Cancel& cancel) {
        file_->seek(0);
        Lzh::compress(file_, targetFile, cancel);
    }

    void FileBasedBinarySource::writeRaw(QFileDevice* targetFile, const Cancel& cancel) {
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
