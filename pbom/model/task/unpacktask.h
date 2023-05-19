#pragma once

#include <QDir>
#include "task.h"
#include "domain/pbodocument.h"
#include "io/settings/fileconflictresolutionmode.h"

namespace pboman3::model::task {
    using namespace domain;

    class UnpackTask : public Task {
    public:
        UnpackTask(QString pboPath, const QString& outputDir);

        void execute(const Cancel& cancel) override;

        friend QDebug operator <<(QDebug debug, const UnpackTask& task);

    private:
        const QString pboPath_;
        const QDir outputDir_;

        bool tryReadPboHeader(QSharedPointer<PboDocument>* document);

        bool tryCreatePboDir(QDir* dir);

        bool tryCreateEntryDir(const QDir& pboDir, const QSharedPointer<PboNode>& entry);

        void extractPboConfig(const PboDocument& document, const QDir& dir, io::FileConflictResolutionMode::Enum conflictResolutionMode);
    };
}
