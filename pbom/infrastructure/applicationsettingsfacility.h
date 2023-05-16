#pragma once

#include <QSharedPointer>
#include "applicationsettings.h"

namespace pboman3::infrastructure {
    class ApplicationSettingsFacility {
    public:
        virtual ~ApplicationSettingsFacility() = default;

        virtual QSharedPointer<ApplicationSettings> readSettings() const = 0;

        virtual void writeSettings(const ApplicationSettings* settings) = 0;
    };
}
