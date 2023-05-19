#include <type_traits>
#include <QFileInfo>
#include "fileconflictresolutionpolicy.h"
#include "diskaccessexception.h"
#include "util/filenames.h"
#include "util/log.h"

#define LOG(...) LOGGER("io/FileConflictResolutionPolicy", __VA_ARGS__)

namespace pboman3::io {
    FileConflictResolutionPolicy::FileConflictResolutionPolicy(FileConflictResolutionMode::Enum conflictResolutionMode)
        : conflictResolutionMode_(conflictResolutionMode) {
    }

    QString FileConflictResolutionPolicy::resolvePotentialConflicts(const QString& filePath) const {
        LOG(info, "Resolving the path:", filePath)
        const QFileInfo fi(filePath);
        if (fi.exists()) {
            switch (conflictResolutionMode_) {
                case FileConflictResolutionMode::Enum::Abort: {
                    LOG(info, "File already exists - aborting")
                    throw DiskAccessException("File already exists.", filePath);
                }
                case FileConflictResolutionMode::Enum::Copy: {
                    auto copyFileName = getCopyFileName(filePath);
                    LOG(info, "File already exists - using the copy:", copyFileName)
                    return copyFileName;
                }
                case FileConflictResolutionMode::Enum::Overwrite: {
                    LOG(info, "File already exists - overwriting")
                    //do nothing
                }
            }
        }
        return filePath;
    }

    QString FileConflictResolutionPolicy::getCopyFileName(const QString& fileName) {
        for (int i = 1; i < std::numeric_limits<int>::max(); i++) {
            auto copyName = util::FileNames::getCopyFileName(fileName, i);
            const QFileInfo fi(copyName);
            if (!fi.exists())
                return copyName;
        }
        throw AppException("Could not pick a copy file name");
    }
}
