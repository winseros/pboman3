#pragma once

#include "io/pboioexception.h"

namespace pboman3 {
    class LzhDecompressionException : public PboIoException {
    public:
        LzhDecompressionException(QString message);

        friend QDebug operator<<(QDebug debug, const PboIoException& ex);

        void raise() const override;

        QException* clone() const override;
    };
}
