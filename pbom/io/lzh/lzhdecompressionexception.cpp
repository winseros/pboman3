#include "lzhdecompressionexception.h"
#include <QDebug>

namespace pboman3 {
    LzhDecompressionException::LzhDecompressionException(QString message) :
        AppException(std::move(message)) {
    }

    QDebug operator<<(QDebug debug, const LzhDecompressionException& ex) {
        return debug << "LzhDecompressionException(" << ex.message() << ")";
    }

    void LzhDecompressionException::raise() const {
        throw* this;
    }

    QException* LzhDecompressionException::clone() const {
        return new LzhDecompressionException(*this);
    }
}
