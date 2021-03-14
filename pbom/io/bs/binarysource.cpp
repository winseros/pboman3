#include "binarysource.h"
#include <QFile>

namespace pboman3 {
    BinarySource::BinarySource(const QString& path)
        : path_(path) {
        file_ = new QFile(path);
        assert(file_->open(QIODeviceBase::ReadOnly) && "Must be able to open this");
    }

    BinarySource::~BinarySource() {
        file_->close();
        delete file_;
    }

    const QString& BinarySource::path() const {
        return path_;
    }
}
