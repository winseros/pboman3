#pragma once

#include "taskwindowmodel.h"
#include "io/fileconflictresolutionmode.h"

namespace pboman3::model::task {
    class PackWindowModel: public TaskWindowModel{
    public:
        PackWindowModel(const QStringList& folders, const QString& outputDir, bool besideInput,
                        io::FileConflictResolutionMode::Enum fileConflictResolutionMode);
    };
}
