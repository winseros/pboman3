#include "binarysourcebase.h"
#include <QFile>
#include "io/diskaccessexception.h"

namespace pboman3::io {
    BinarySourceBase::BinarySourceBase(QString path)
        : path_(std::move(path)) {
        file_ = new QFile(path_);
    }

    BinarySourceBase::~BinarySourceBase() {
        delete file_;
    }

    void BinarySourceBase::open() const {
        if (!file_->open(QIODeviceBase::ReadOnly))
            throw DiskAccessException("Can not open the file. Check you have enough permissions and the file is not locked by another process.", path_);
    }

    void BinarySourceBase::close() const {
        file_->close();
    }

    bool BinarySourceBase::isOpen() const {
        return file_->isOpen();
    }

    const QString& BinarySourceBase::path() const {
        return path_;
    }
}
