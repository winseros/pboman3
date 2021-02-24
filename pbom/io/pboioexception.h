#pragma once

#include <QDebug>
#include <QException>

namespace pboman3 {
    class PboIoException : public QException {
    public:
        PboIoException(QString message);

        const QString& message() const;

        friend QDebug operator<<(QDebug debug, const PboIoException& ex);

        void raise() const override;

        QException* clone() const override;
    private:
        QString message_;
    };
}
