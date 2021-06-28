#include "binarysource.h"
#include <QFile>
#include "io/pboioexception.h"

namespace pboman3 {
    BinarySource::BinarySource(QString path)
        : path_(std::move(path)) {
        file_ = new QFile(path_);
    }

    BinarySource::~BinarySource() {
        delete file_;
    }

    void BinarySource::open() const {
        if (!file_->open(QIODeviceBase::ReadOnly))
            throw PboIoException("Can not open the file. Check you have enough permissions and the file is not locked by another process.", path_);
    }

    void BinarySource::close() const {
        file_->close();
    }

    bool BinarySource::isOpen() const {
        return file_->isOpen();
    }

    const QString& BinarySource::path() const {
        return path_;
    }
}
