#include "pbotreeexception.h"

namespace pboman3 {
    PboTreeException::PboTreeException(QString message)
        : AppException(std::move(message)) {
    }

    QDebug operator<<(QDebug debug, const PboTreeException& ex) {
        return debug << "PboTreeException: " << ex.message();
    }

    void PboTreeException::raise() const {
        throw*this;
    }

    QException* PboTreeException::clone() const {
        return new PboTreeException(*this);
    }
}
