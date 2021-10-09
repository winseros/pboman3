#pragma once

#include "taskwindowmodel.h"

namespace pboman3 {
    class PackWindowModel: public TaskWindowModel{
    public:
        PackWindowModel(const QStringList& folders, const QString& targetPath);
    };
}
