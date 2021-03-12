#include "appexception.h"

namespace pboman3 {
    AppException::AppException(QString message)
        : QException(),
        message_(std::move(message)) {
    }

    const QString& AppException::message() const {
        return message_;
    }
}
