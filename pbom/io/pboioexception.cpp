#include "pboioexception.h"

namespace pboman3 {
    PboIoException::PboIoException(QString message)
        : QException(),
          message_(std::move(message)) {
    }

    const QString& PboIoException::message() const {
        return message_;
    }

    QDebug operator<<(QDebug debug, const PboIoException& ex) {
        return debug << "PboIoException: " << ex.message_;
    }


    void PboIoException::raise() const {
        throw* this;
    }

    QException* PboIoException::clone() const {
        return new PboIoException(*this);
    }
}
