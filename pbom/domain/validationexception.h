#pragma once

#include "exception.h"
#include <QDebug>

namespace pboman3::domain {
    class ValidationException : public AppException {
    public:
        ValidationException(QString message);

        PBOMAN_EX_HEADER(ValidationException)
    };
}
