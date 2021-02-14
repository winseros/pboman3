#pragma once

#include <QException>

namespace pboman3 {
    class PboTreeException: public QException {
    public:
        PboTreeException(QString message);

        const QString& message() const;
    private:
        const QString message_;
    };
}
