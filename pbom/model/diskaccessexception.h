#pragma once

#include "io/pboioexception.h"
#include "util/exception.h"

namespace pboman3 {
    class DiskAccessException : public AppException {
    public:
        DiskAccessException(QString message, QString file);

        explicit DiskAccessException(const PboIoException& ex);

        void raise() const override;

        QException* clone() const override;

        const QString& file() const;

    private:
        QString file_;
    };
}
