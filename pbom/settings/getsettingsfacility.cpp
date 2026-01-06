#include "getsettingsfacility.h"
#include "localstorageapplicationsettingsfacility.h"

namespace pboman3::settings {
    QSharedPointer<ApplicationSettingsFacility> GetSettingsFacility() {
        return QSharedPointer<ApplicationSettingsFacility>(new LocalStorageApplicationSettingsFacility);
    }
}
