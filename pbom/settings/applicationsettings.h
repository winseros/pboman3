#pragma once

#include "io/fileconflictresolutionmode.h"
#include "operationcompletebehavior.h"
#include "applicationcolorscheme.h"

namespace pboman3::settings {
    struct ApplicationSettings {
        io::FileConflictResolutionMode::Enum packConflictResolutionMode;

        io::FileConflictResolutionMode::Enum unpackConflictResolutionMode;

        OperationCompleteBehavior::Enum packUnpackOperationCompleteBehavior;

        bool junkFilterEnable;

        ApplicationColorScheme::Enum applicationColorScheme;
    };
}
