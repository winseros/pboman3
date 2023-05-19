#pragma once

#include "applicationsettings.h"

namespace pboman3::io {
    class ApplicationSettingsFacility {
    public:
        virtual ~ApplicationSettingsFacility() = default;

        virtual ApplicationSettings readSettings() const = 0;

        virtual void writeSettings(const ApplicationSettings& settings) = 0;
    };
}
