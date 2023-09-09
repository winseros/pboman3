#pragma once

#include "applicationsettingsfacility.h"
#include <QSharedPointer>

namespace pboman3::io{
    QSharedPointer<ApplicationSettingsFacility> GetSettingsFacility();
}
