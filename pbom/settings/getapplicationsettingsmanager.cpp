#include <QSharedPointer>
#include "getapplicationsettingsmanager.h"
#include "localstorageapplicationsettingsmanager.h"

namespace pboman3::settings {
    QSharedPointer<ApplicationSettingsManager> applicationSettingsManager_ = nullptr;

    ApplicationSettingsManager* GetApplicationSettingsManager() {
        if (applicationSettingsManager_.isNull()) {
            applicationSettingsManager_ = QSharedPointer<ApplicationSettingsManager>(
                new LocalStorageApplicationSettingsManager);
        }
        return applicationSettingsManager_.get();
    }
}
