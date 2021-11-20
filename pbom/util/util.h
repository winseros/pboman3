#pragma once

#include <QString>
#include <functional>

namespace pboman3::util {
    typedef std::function<bool()> Cancel;

    QString GetFileExtension(const QString& fileName);

    QString GetFileNameWithoutExtension(const QString& fileName);

    void SplitByNameAndExtension(const QString& fileName, QString& outFileName, QString& outExtension);
}
