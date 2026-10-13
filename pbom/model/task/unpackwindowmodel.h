#pragma once

#include "taskwindowmodel.h"
#include "io/fileconflictresolutionmode.h"

namespace pboman3::model::task {
    class UnpackWindowModel : public TaskWindowModel {
    public:
        UnpackWindowModel(const QStringList& pboFiles, const QString& outputDir, bool besideInput, bool usePboPrefix,
                          io::FileConflictResolutionMode::Enum fileConflictResolutionMode);
    };
}
