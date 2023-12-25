#include "unpacktaskbackend.h"
#include "io/diskaccessexception.h"
#include "io/fileconflictresolutionpolicy.h"
#include "util/log.h"

#define LOG(...) LOGGER("io/bb/UnpackTaskBackend", __VA_ARGS__)

namespace pboman3::io {
    UnpackTaskBackend::UnpackTaskBackend(const QDir& folder, FileConflictResolutionMode::Enum conflictResolutionMode)
        : UnpackBackend(folder),
          onError_(nullptr),
          onProgress_(nullptr) {
        conflictResolutionPolicy_ = QSharedPointer<FileConflictResolutionPolicy>(
            new FileConflictResolutionPolicy(conflictResolutionMode));
    }

    void UnpackTaskBackend::setOnError(std::function<void(const QString&)>* callback) {
        onError_ = callback;
    }

    void UnpackTaskBackend::setOnProgress(std::function<void()>* callback) {
        onProgress_ = callback;
    }

    void UnpackTaskBackend::unpackFileNode(const PboNode* rootNode, const PboNode* childNode,
                                           const Cancel& cancel) const {
        LOG(debug, "Unpack the node", childNode->title())

        QString filePath;
        try {
            filePath = nodeFileSystem_->allocatePath(rootNode, childNode);
            filePath = conflictResolutionPolicy_->resolvePotentialConflicts(filePath);
        } catch (const DiskAccessException& ex) {
            LOG(warning, ex)
            //remove the "." symbol from the end
            error(ex.message().left(ex.message().length() - 1) + " | " + ex.file());
            progress();
            return;
        }

        if (cancel())
            return;

        QFile file(filePath);
        if (!file.open(QIODeviceBase::ReadWrite)) {
            //WriteOnly won't work for LZH unpacking
            LOG(warning, "Can not access the file:", file.fileName())
            error("Can could not write to the file | " + file.fileName());
            progress();
            return;
        }

        LOG(debug, "Writing to file system")
        const auto bsClose = qScopeGuard([&childNode] { if (childNode->binarySource->isOpen()) childNode->binarySource->close(); });
        childNode->binarySource->open();
        childNode->binarySource->writeToFs(&file, cancel);

        file.close();

        progress();
    }

    void UnpackTaskBackend::error(const QString& error) const {
        if (onError_) {
            (*onError_)(error);
        }
    }

    void UnpackTaskBackend::progress() const {
        if (onProgress_) {
            (*onProgress_)();
        }
    }
}
