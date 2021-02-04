#pragma once

#include "pboentry.h"

namespace pboman3 {
    struct ChangeAdd {
        const QString systemFilePath;
        const QString pboFilePath;

        ChangeAdd(QString filePath, QString pboPath);
    };

    struct ChangeBase {
        const PboEntry_* entry;
        ChangeBase(const PboEntry_* entry);
        virtual ~ChangeBase() = default;
    };

    struct ChangeMove : public ChangeBase { 
        const QString pboFilePath;
        ChangeMove(const PboEntry_* entry, QString newPboPath);
    };

    struct ChangeDelete : public ChangeBase {
        ChangeDelete(const PboEntry_* entry);
    };
}
