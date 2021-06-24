#include "pboioexception.h"

namespace pboman3 {
    PboIoException::PboIoException(QString message, QString file)
        : AppException(std::move(message)),
        file_(std::move(file)){
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

    QString PboIoException::file() const {
        return file_;
    }
}
