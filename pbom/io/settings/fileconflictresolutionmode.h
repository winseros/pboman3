#pragma once

#include "util/enum.h"
#include <QVariant>

namespace pboman3::io {
    struct FileConflictResolutionMode {
        enum class Enum {
            Abort = 0,
            Copy = 1,
            Overwrite = 2
        };

        static bool tryRead(const QVariant& value, Enum& result) {
            return util::TryReadEnum<Enum, Enum::Abort, Enum::Copy, Enum::Overwrite>(value, result);
        }
    };
}
