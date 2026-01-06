#pragma once

#include "applicationsettingsfacility.h"

namespace pboman3::settings {
    class LocalStorageApplicationSettingsFacility : public ApplicationSettingsFacility {
    public:
        static void purge();

        [[nodiscard]] ApplicationSettings readSettings() const override;

        void writeSettings(const ApplicationSettings& settings) override;
    };
}
