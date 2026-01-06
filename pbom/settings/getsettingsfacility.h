#pragma once

#include "applicationsettingsfacility.h"
#include <QSharedPointer>

namespace pboman3::settings{
    QSharedPointer<ApplicationSettingsFacility> GetSettingsFacility();
}
