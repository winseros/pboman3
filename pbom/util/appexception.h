#pragma once

#include <QException>

namespace pboman3 {
    class AppException : public QException {
    public:
        AppException(QString message);

        const QString& message() const;

    private:
        const QString message_;
    };
}
