#pragma once

#include <QString>

namespace pboman3::util {
    class FileNames {
    public:
        static QString getCopyFolderName(const QString& desiredFolderName, int suffix);

        static QString getCopyFileName(const QString& desiredFileName, int suffix);

        static QString getFileExtension(const QString& fileName);

        static QString getFileNameWithoutExtension(const QString& fileName);

        static void splitByNameAndExtension(const QString& fileName, QString& outFileName, QString& outExtension);
    };
}
