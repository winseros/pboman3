#include "validationexception.h"

namespace pboman3::domain {
    ValidationException::ValidationException(QString message)
        : AppException(std::move(message)) {
    }

    QDebug operator<<(QDebug debug, const ValidationException& ex) {
        return debug << "ValidationException(" << ex.message_ << ")";
    }

    void ValidationException::raise() const {
        throw* this;
    }

    QException* ValidationException::clone() const {
        return new ValidationException(*this);
    }
}
