#pragma once

#include "exception.h"

namespace pboman3::io {
    class PboFileFormatException : public AppException {
    public:
        explicit PboFileFormatException(QString message);

        void raise() const override;

        QException* clone() const override;

        friend QDebug operator<<(QDebug debug, const PboFileFormatException& ex);
    };
}
