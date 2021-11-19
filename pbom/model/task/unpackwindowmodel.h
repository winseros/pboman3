#pragma once

#include "taskwindowmodel.h"

namespace pboman3::model {
    class UnpackWindowModel: public TaskWindowModel{
    public:
        UnpackWindowModel(const QStringList& pboFiles, const QString& outputDir);
    };
}
