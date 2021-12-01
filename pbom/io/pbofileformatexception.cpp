#include "pbofileformatexception.h"
#include <QDebug>

namespace pboman3::io {
    PboFileFormatException::PboFileFormatException(QString message)
        : AppException(std::move(message)) {
    }

    void PboFileFormatException::raise() const {
        throw *this;
    }

    QException* PboFileFormatException::clone() const {
        return new PboFileFormatException(*this);
    }

    QDebug operator<<(QDebug debug, const PboFileFormatException& ex) {
        return debug << "PboFileFormatException(" << ex.message_ << ")";
    }
}
