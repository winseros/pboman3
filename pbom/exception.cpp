#include "exception.h"
#include <QDebug>

namespace pboman3 {
    AppException::AppException(QString message)
        : QException(),
        message_(std::move(message)) {
    }

    const QString& AppException::message() const {
        return message_;
    }

    QDebug& operator<<(QDebug& debug, const AppException& ex) {
        return ex.dump(debug);
    }

    QDebug& AppException::dump(QDebug& debug) const {
        return debug << "AppException(" << message_ << ")";
    }

    void AppException::raise() const {
        throw* this;
    }

    QException* AppException::clone() const {
        return new AppException(*this);
    }


    InvalidOperationException::InvalidOperationException(QString message)
        : AppException(std::move(message)) {
    }

    PBOMAN_EX_IMPL_DEFAULT(InvalidOperationException)
}
