#pragma once

#include "exception.h"

namespace pboman3::io {
    class DiskAccessException : public AppException {
    public:
        DiskAccessException(QString message, QString file);

        const QString& file() const;

        PBOMAN_EX_HEADER(DiskAccessException)

    private:
        QString file_;
    };
}
