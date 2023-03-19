#include "lzhdecompressionexception.h"
#include <QDebug>

namespace pboman3::io {
    LzhDecompressionException::LzhDecompressionException(QString message) :
        AppException(std::move(message)) {
    }

    PBOMAN_EX_IMPL_DEFAULT(LzhDecompressionException)
}
