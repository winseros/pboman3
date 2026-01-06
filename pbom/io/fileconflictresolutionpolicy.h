#pragma once

#include <QString>
#include "fileconflictresolutionmode.h"

namespace pboman3::io {
    class FileConflictResolutionPolicy {
    public:
        explicit FileConflictResolutionPolicy(FileConflictResolutionMode::Enum conflictResolutionMode);

        QString resolvePotentialConflicts(const QString& filePath) const;

    private:
        FileConflictResolutionMode::Enum conflictResolutionMode_;

        static QString getCopyFileName(const QString& fileName);
    };
}
