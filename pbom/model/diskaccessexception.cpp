#include "diskaccessexception.h"
#include <QDebug>

namespace pboman3 {
    DiskAccessException::DiskAccessException(QString message)
        : AppException(std::move(message)) {
    }

    QDebug operator<<(QDebug debug, const DiskAccessException& ex) {
        return debug << "DiskAccessException(" << ex.message_ << ")";
    }

    void DiskAccessException::raise() const {
        throw *this;
    }

    QException* DiskAccessException::clone() const {
        return new DiskAccessException(*this);
    }
}
