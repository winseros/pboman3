#pragma once

#include "taskwindowmodel.h"

namespace pboman3::model::task {
    class PackWindowModel: public TaskWindowModel{
    public:
        PackWindowModel(const QStringList& folders, const QString& outputDir);
    };
}
