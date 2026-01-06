#pragma once

#include "unpackbackend.h"
#include "io/fileconflictresolutionmode.h"
#include "io/fileconflictresolutionpolicy.h"
#include <QSharedPointer>

namespace pboman3::io {
    class UnpackTaskBackend : public UnpackBackend {

    public:
        UnpackTaskBackend(const QDir& folder, FileConflictResolutionMode::Enum conflictResolutionMode);

        void setOnError(std::function<void(const QString&)>* callback);

        void setOnProgress(std::function<void()>* callback);

    protected:
        void unpackFileNode(const PboNode* rootNode, const PboNode* childNode, const Cancel& cancel) const override;

    private:
        std::function<void(const QString&)>* onError_;
        std::function<void()>* onProgress_;
        QSharedPointer<FileConflictResolutionPolicy> conflictResolutionPolicy_;

        void error(const QString& error) const;

        void progress() const;
    };
}
