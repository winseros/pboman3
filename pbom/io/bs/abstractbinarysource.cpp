#include "abstractbinarysource.h"
#include <QFile>
#include "io/diskaccessexception.h"

namespace pboman3::io {
    AbstractBinarySource::AbstractBinarySource(QString path)
        : path_(std::move(path)) {
        file_ = new QFile(path_);
    }

    AbstractBinarySource::~AbstractBinarySource() {
        delete file_;
    }

    void AbstractBinarySource::open() const {
        if (isOpen())
            throw InvalidOperationException("The source is already opened");

        if (!file_->open(QIODeviceBase::ReadOnly))
            throw DiskAccessException("Can not open the file. Check you have enough permissions, the file exists and is not locked by another process.", path_);
    }

    void AbstractBinarySource::close() const {
        if (!isOpen())
            throw InvalidOperationException("The source is not opened");

        file_->close();
    }

    bool AbstractBinarySource::isOpen() const {
        return file_->isOpen();
    }

    const QString& AbstractBinarySource::path() const {
        return path_;
    }
}
