#pragma once

#include "util/exception.h"

namespace pboman3::io {
    class PboFileFormatException : public AppException {
    public:
        explicit PboFileFormatException(QString message);

        friend QDebug operator<<(QDebug debug, const PboFileFormatException& ex);

        QException* clone() const override;
    };
}
