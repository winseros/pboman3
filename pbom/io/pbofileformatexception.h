#pragma once

#include "exception.h"

namespace pboman3::io {
    class PboFileFormatException : public AppException {
    public:
        explicit PboFileFormatException(QString message);

        PBOMAN_EX_HEADER(PboFileFormatException)
    };
}
