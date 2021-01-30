#pragma once

#include <QString>

namespace pboman3 {
    struct PboHeader {
        const QString name;
        const QString value;

        PboHeader(QString name, QString value);

        bool isBoundary() const;
    };
}
