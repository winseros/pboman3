#include "diskaccessexception.h"
#include <QDebug>

namespace pboman3::io {
    DiskAccessException::DiskAccessException(QString message, QString file)
        : AppException(std::move(message)),
          file_(std::move(file)) {
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

    QDebug operator<<(QDebug debug, const DiskAccessException& ex) {
        return debug << "DiskAccessException(" << ex.message_ << ")";
    }
}
