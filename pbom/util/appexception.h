#pragma once

#include <QException>

namespace pboman3 {
    class AppException : public QException {
    public:
        AppException(QString message);

        const QString& message() const;

        friend QDebug operator<<(QDebug debug, const AppException& ex);

        void raise() const override;

        QException* clone() const override;

    private:
        const QString message_;
    };
}
