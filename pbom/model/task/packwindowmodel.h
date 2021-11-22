#pragma once

#include "taskwindowmodel.h"

namespace pboman3::model {
    class PackWindowModel: public TaskWindowModel{
    public:
        PackWindowModel(const QStringList& folders, const QString& outputDir);
    };
}
