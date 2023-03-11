#include "pbofileformatexception.h"
#include <QDebug>

namespace pboman3::io {
    PboFileFormatException::PboFileFormatException(QString message)
        : AppException(std::move(message)) {
    }

    PBOMAN_EX_IMPL_DEFAULT(PboFileFormatException)
}
