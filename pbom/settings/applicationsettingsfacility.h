#pragma once

#include "applicationsettings.h"

namespace pboman3::settings {
    class ApplicationSettingsFacility {
    public:
        virtual ~ApplicationSettingsFacility() = default;

        [[nodiscard]] virtual ApplicationSettings readSettings() const = 0;

        virtual void writeSettings(const ApplicationSettings& settings) = 0;
    };
}
