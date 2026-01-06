#pragma once

#include "util/enum.h"

namespace pboman3::settings {
    struct ApplicationColorScheme {
        enum class Enum {
            Auto = 0,
            Light = 1,
            Dark
        };

        static bool tryRead(const QVariant& value, Enum& result) {
            return util::TryReadEnum<Enum, Enum::Auto, Enum::Light, Enum::Dark>(value, result);
        }
    };
}
