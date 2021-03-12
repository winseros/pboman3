#pragma once

#include <QDebug>
#include "util/appexception.h"

namespace pboman3 {
    class PboTreeException : public AppException {
    public:
        PboTreeException(QString message);

        friend QDebug operator<<(QDebug debug, const PboTreeException& ex);

        void raise() const override;

        QException* clone() const override;
    };
}
