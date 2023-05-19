#include <type_traits>
#include <QFileInfo>
#include "filefactory.h"
#include "diskaccessexception.h"
#include "util/filenames.h"
#include "util/log.h"

#define LOG(...) LOGGER("io/FileFactory", __VA_ARGS__)

namespace pboman3::io {
    FileFactory::FileFactory(FileConflictResolutionMode::Enum conflictResolutionMode)
        : conflictResolutionMode_(conflictResolutionMode) {
    }

    QSharedPointer<QFile> FileFactory::createFile(const QString& filePath) const {
        LOG(info, "Creating the file:", filePath)
        QSharedPointer<QFile> file(new QFile(filePath));
        if (file->exists()) {
            switch (conflictResolutionMode_) {
                case FileConflictResolutionMode::Enum::Abort: {
                    LOG(info, "File already exists - aborting")
                    throw DiskAccessException("File already exists", filePath);
                }
                case FileConflictResolutionMode::Enum::Copy: {
                    const auto copyFileName = getCopyFileName(filePath);
                    LOG(info, "File already exists - making the copy:", copyFileName)
                    QSharedPointer<QFile> copyFile(new QFile(copyFileName));
                    return copyFile;
                }
                case FileConflictResolutionMode::Enum::Overwrite: {
                    LOG(info, "File already exists - overwriting")
                    //do nothing
                }
            }
        }
        return file;
    }

    QString FileFactory::resolvePath(const QString& filePath) const {
        LOG(info, "Resolving the path:", filePath)
        const QFileInfo fi(filePath);
        if (fi.exists()) {
            switch (conflictResolutionMode_) {
                case FileConflictResolutionMode::Enum::Abort: {
                    LOG(info, "File already exists - aborting")
                    throw DiskAccessException("File already exists", filePath);
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

    QString FileFactory::getCopyFileName(const QString& fileName) {
        for (int i = 1; i < std::numeric_limits<int>::max(); i++) {
            auto copyName = util::FileNames::getCopyFileName(fileName, i);
            const QFileInfo fi(copyName);
            if (!fi.exists())
                return copyName;
        }
        throw AppException("Could not pick a copy file name");
    }
}
