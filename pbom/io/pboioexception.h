#pragma once

#include <QDebug>
#include "util/appexception.h"

namespace pboman3 {
    class PboIoException : public AppException {
    public:
        PboIoException(QString message, QString file);

        friend QDebug operator<<(QDebug debug, const PboIoException& ex);

        void raise() const override;

        QException* clone() const override;

        QString file() const;

    private:
        QString file_;
    };
}
