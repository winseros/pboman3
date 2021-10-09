#include "pbofileformatexception.h"
#include <QDebug>

namespace pboman3 {
    PboFileFormatException::PboFileFormatException(QString message)
        : AppException(std::move(message)) {
    }

    QDebug operator<<(QDebug debug, const PboFileFormatException& ex) {
        return debug << "PboFileFormatException(" << ex.message_ << ")";
    }

    QException* PboFileFormatException::clone() const {
        return new PboFileFormatException(*this);
    }
}
