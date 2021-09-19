#pragma once

#include <QDir>
#include "task.h"
#include "io/pboheaderreader.h"

namespace pboman3 {
    class UnpackTask : public Task {
    public:
        UnpackTask(QString pboPath, const QString& targetPath);

        void execute(const Cancel& cancel) override;

    private:
        const QString pboPath_;

        const QDir targetDir_;

        bool tryReadPboHeader(PboFileHeader* header);

        bool tryCreatePboDir(QDir* dir);

        bool tryCreateEntryDir(const QDir& pboDir, const QSharedPointer<PboEntry>& entry);
    };
}
