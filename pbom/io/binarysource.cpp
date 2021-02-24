#include "binarysource.h"
#include <QFile>
#include <QVariant>

namespace pboman3 {
    BinarySource::BinarySource(const QString& path) {
        file_ = new QFile(path);
        assert(file_->open(QIODeviceBase::ReadOnly) && "Must be able to open this");
    }

    BinarySource::~BinarySource() {
        file_->close();
        delete file_;
    }


    FileBasedBinarySource::FileBasedBinarySource(const QString& path, size_t bufferSize)
        : BinarySource(path),
          bufferSize_(bufferSize) {
    }

    void FileBasedBinarySource::writeTo(QFileDevice* targetFile, const Cancel& cancel) {
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


    PboBasedBinarySource::PboBasedBinarySource(const QString& path, const PboDataInfo& dataInfo, size_t bufferSize)
        : BinarySource(path),
          dataInfo_(dataInfo),
          bufferSize_(bufferSize) {
    }

    void PboBasedBinarySource::writeTo(QFileDevice* targetFile, const Cancel& cancel) {
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

}
