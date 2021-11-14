#pragma once

#include "util/exception.h"

namespace pboman3::io {
    class DiskAccessException : public AppException {
    public:
        DiskAccessException(QString message, QString file);

        void raise() const override;

        QException* clone() const override;

        const QString& file() const;

    private:
        QString file_;
    };
}
