#pragma once

#include "applicationsettingsfacility.h"

namespace pboman3::io {
    class LocalStorageApplicationSettingsFacility : public ApplicationSettingsFacility {
    public:
        void purge();

        ApplicationSettings readSettings() const override;

        void writeSettings(const ApplicationSettings& settings) override;
    };
}
