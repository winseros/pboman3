#pragma once

#include <QDebug>
#include "util/appexception.h"

namespace pboman3 {
    class AddEntryCancelException : public AppException {
    public:
        AddEntryCancelException(QString message);

        friend QDebug operator<<(QDebug debug, const AddEntryCancelException& ex);

        void raise() const override;

        QException* clone() const override;
    };
}
