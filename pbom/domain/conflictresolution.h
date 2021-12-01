#pragma once

namespace pboman3::domain {
    enum class ConflictResolution {
        Unset = -1,
        Skip = 0,
        Copy = 1,
        Replace = 2,
    };
}
