#pragma once

#include <QDir>
#include "task.h"
#include "domain/pbonode.h"
#include "io/settings/fileconflictresolutionmode.h"

namespace pboman3::model::task {
    using namespace domain;
    using namespace io;

    class PackTask : public Task {
    public:
        PackTask(QString folder, QString outputDir, FileConflictResolutionMode::Enum fileConflictResolutionMode);

        void execute(const Cancel& cancel) override;

        friend QDebug operator<<(QDebug debug, const PackTask& task);

    private:
        QString folder_;
        QString outputDir_;
        FileConflictResolutionMode::Enum fileConflictResolutionMode_;

        qint32 collectDir(const QDir& dirEntry, const QDir& rootDir, PboNode& rootNode, const Cancel& cancel) const;

        qint32 collectFile(const QFileInfo& fileEntry, const QDir& rootDir, PboNode& rootNode) const;
    };
}
