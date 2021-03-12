#include "pboioexception.h"

namespace pboman3 {
    PboIoException::PboIoException(QString message)
        : AppException(std::move(message)) {
    }

    QDebug operator<<(QDebug debug, const PboIoException& ex) {
        return debug << "PboIoException: " << ex.message();
    }

    void PboIoException::raise() const {
        throw* this;
    }

    QException* PboIoException::clone() const {
        return new PboIoException(*this);
    }
}
