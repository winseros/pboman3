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

    protected:
        const QString message_;
    };

    class InvalidOperationException : public AppException {
    public:
        InvalidOperationException(QString message);

        friend QDebug operator<<(QDebug debug, const InvalidOperationException& ex);

        void raise() const override;

        QException* clone() const override;
    };
}
