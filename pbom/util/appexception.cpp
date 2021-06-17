#include "appexception.h"
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
        return debug << "AppException: " << ex.message();
    }

    void AppException::raise() const {
        throw* this;
    }

    QException* AppException::clone() const {
        return new AppException(*this);
    }

}
