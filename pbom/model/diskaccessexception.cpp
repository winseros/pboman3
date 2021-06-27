#include "diskaccessexception.h"
#include <QDebug>

namespace pboman3 {
    DiskAccessException::DiskAccessException(QString message, QString file)
        : AppException(std::move(message)),
          file_(std::move(file)) {
    }

    DiskAccessException::DiskAccessException(const PboIoException& ex):
        AppException(ex.message()),
        file_(ex.file()) {
    }

    void DiskAccessException::raise() const {
        throw *this;
    }

    QException* DiskAccessException::clone() const {
        return new DiskAccessException(*this);
    }

    const QString& DiskAccessException::file() const {
        return file_;
    }
}
