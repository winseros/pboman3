#pragma once

#include <QString>

namespace pboman3::io {
    struct PboHeaderEntity {
        static PboHeaderEntity makeBoundary();

        const QString name;
        const QString value;

        PboHeaderEntity(QString name, QString value);

        bool isBoundary() const;
    };
}
