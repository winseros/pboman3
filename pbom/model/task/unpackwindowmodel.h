#pragma once

#include "taskwindowmodel.h"

namespace pboman3::model::task {
    class UnpackWindowModel: public TaskWindowModel{
    public:
        UnpackWindowModel(const QStringList& pboFiles, const QString& outputDir);
    };
}
