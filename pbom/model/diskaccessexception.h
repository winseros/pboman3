#pragma once

#include "util/appexception.h"

namespace pboman3 {
    class DiskAccessException : public AppException {
    public:
        explicit DiskAccessException(QString message);

        friend QDebug operator<<(QDebug debug, const DiskAccessException& ex);

        void raise() const override;

        QException* clone() const override;
    };
}
