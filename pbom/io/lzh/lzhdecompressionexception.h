#pragma once

#include "util/exception.h"

namespace pboman3 {
    class LzhDecompressionException : public AppException {
    public:
        LzhDecompressionException(QString message);

        friend QDebug operator<<(QDebug debug, const LzhDecompressionException& ex);

        void raise() const override;

        QException* clone() const override;
    };
}
