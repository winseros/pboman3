#pragma once

#include <QString>
#include <functional>

namespace pboman3 {
    typedef std::function<bool()> Cancel;

    QString GetFileExtension(const QString& fileName);

    QString GetFileNameWithoutExtension(const QString& fileName);
}
