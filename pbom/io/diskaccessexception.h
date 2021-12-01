#pragma once

#include "exception.h"

namespace pboman3::io {
    class DiskAccessException : public AppException {
    public:
        DiskAccessException(QString message, QString file);

        void raise() const override;

        QException* clone() const override;

        const QString& file() const;

        friend QDebug operator<<(QDebug debug, const DiskAccessException& ex);

    private:
        QString file_;
    };
}
