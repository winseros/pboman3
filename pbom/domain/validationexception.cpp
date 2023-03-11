#include "validationexception.h"

namespace pboman3::domain {
    ValidationException::ValidationException(QString message)
        : AppException(std::move(message)) {
    }

    PBOMAN_EX_IMPL_DEFAULT(ValidationException)
}
