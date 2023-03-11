#pragma once

#include "exception.h"

namespace pboman3::io {
    class LzhDecompressionException : public AppException {
    public:
        LzhDecompressionException(QString message);

        PBOMAN_EX_HEADER(LzhDecompressionException)
    };
}
