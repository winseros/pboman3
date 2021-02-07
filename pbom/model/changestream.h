#pragma once

#include "pboentry.h"
#include <QSharedPointer>

namespace pboman3 {
    struct ChangeAdd {
        const QString systemFilePath;
        const QString pboFilePath;

        ChangeAdd(QString filePath, QString pboPath);
    };
}
