#pragma once

#include <QVariant>
#include "util/enum.h"

namespace pboman3::infrastructure {
    struct OperationCompleteBehavior {
        enum class Enum {
            KeepWindow = 0,
            CloseWindow = 1
        };

        static bool tryRead(const QVariant& value, Enum& result) {
            return util::TryReadEnum<Enum, Enum::KeepWindow, Enum::CloseWindow>(value, result);
        }
    };
}
