#pragma once

#include "fileconflictresolutionmode.h"
#include "operationcompletebehavior.h"

namespace pboman3::io {
    struct ApplicationSettings {
        FileConflictResolutionMode::Enum packConflictResolutionMode;

        FileConflictResolutionMode::Enum unpackConflictResolutionMode;

        OperationCompleteBehavior::Enum packUnpackOperationCompleteBehavior;

        bool junkFilterEnable;
    };
}
