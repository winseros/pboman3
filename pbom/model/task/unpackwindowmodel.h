#pragma once

#include "taskwindowmodel.h"
#include "io/settings/fileconflictresolutionmode.h"

namespace pboman3::model::task {
    class UnpackWindowModel: public TaskWindowModel{
    public:
        UnpackWindowModel(const QStringList& pboFiles, const QString& outputDir, io::FileConflictResolutionMode::Enum fileConflictResolutionMode);
    };
}
