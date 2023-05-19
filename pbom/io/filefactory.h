#pragma once

#include <QSharedPointer>
#include <QFile>
#include <QString>
#include "settings/fileconflictresolutionmode.h"

namespace pboman3::io {
    class FileFactory {
    public:
        explicit FileFactory(FileConflictResolutionMode::Enum conflictResolutionMode);

        QSharedPointer<QFile> createFile(const QString& filePath) const;

        QString resolvePath(const QString& filePath) const;

    private:
        FileConflictResolutionMode::Enum conflictResolutionMode_;

        static QString getCopyFileName(const QString& fileName);
    };
}
