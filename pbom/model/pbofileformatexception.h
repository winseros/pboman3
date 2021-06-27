#pragma once

#include "util/appexception.h"

namespace pboman3 {
    class PboFileFormatException : public AppException {
    public:
        explicit PboFileFormatException(QString message);

        friend QDebug operator<<(QDebug debug, const PboFileFormatException& ex);

        QException* clone() const override;
    };
}
