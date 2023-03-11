#include "diskaccessexception.h"
#include <QDebug>

namespace pboman3::io {
    DiskAccessException::DiskAccessException(QString message, QString file)
        : AppException(std::move(message)),
          file_(std::move(file)) {
    }

    PBOMAN_EX_IMPL_DEFAULT(DiskAccessException)

    const QString& DiskAccessException::file() const {
        return file_;
    }
}
