#pragma once

#include "applicationsettingsmanager.h"

namespace pboman3::settings {
    class LocalStorageApplicationSettingsManager : public ApplicationSettingsManager {
        Q_OBJECT

    public:
        static void purge();

        [[nodiscard]] ApplicationSettings readSettings() const override;

        void writeSettings(const ApplicationSettings& settings) override;
    };
}
