#pragma once

#include "applicationsettings.h"

namespace pboman3::settings {
    class ApplicationSettingsManager : public QObject {
        Q_OBJECT

    public:
        [[nodiscard]] virtual ApplicationSettings readSettings() const = 0;

        virtual void writeSettings(const ApplicationSettings& settings) = 0;

    signals:
        void settingsChanged(const ApplicationSettings& settings);
    };
}
