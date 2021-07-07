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

    QDebug operator<<(QDebug debug, const AppException& ex) {
        return debug << "AppException(" << ex.message_ << ")";
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

    QDebug operator<<(QDebug debug, const InvalidOperationException& ex) {
        return debug << "InvalidOperationException(" << ex.message_ << ")";
    }

    void InvalidOperationException::raise() const {
        throw* this;
    }

    QException* InvalidOperationException::clone() const {
        return new InvalidOperationException(*this);
    }
}
