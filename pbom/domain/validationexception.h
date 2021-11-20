#pragma once

#include "exception.h"
#include <QDebug>

namespace pboman3::domain {
    class ValidationException : public AppException {
    public:
        ValidationException(QString message);

        friend QDebug operator<<(QDebug debug, const ValidationException& ex);

        void raise() const override;

        QException* clone() const override;
    };
}
