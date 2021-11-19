#pragma once

#include <QString>

namespace pboman3::io {
    struct PboHeader {
        static PboHeader makeBoundary();

        const QString name;
        const QString value;

        PboHeader(QString name, QString value);

        bool isBoundary() const;
    };
}
