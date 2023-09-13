#include "getsettingsfacility.h"
#include "localstorageapplicationsettingsfacility.h"

namespace pboman3::io {
    QSharedPointer<ApplicationSettingsFacility> GetSettingsFacility() {
        return QSharedPointer<ApplicationSettingsFacility>(new LocalStorageApplicationSettingsFacility);
    }
}
