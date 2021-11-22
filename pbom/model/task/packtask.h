#pragma once

#include <QDir>
#include "task.h"
#include "model/interactionparcel.h"

namespace pboman3::model {
    using namespace domain;

    class PackTask : public Task {
    public:
        PackTask(QString folder, QString outputDir);

        void execute(const Cancel& cancel) override;

        friend QDebug operator<<(QDebug debug, const PackTask& task);

    private:
        const QString folder_;
        const QString outputDir_;

        qint32 collectDir(const QFileInfo& dirEntry, const QDir& rootDir, PboNode& rootNode, const Cancel& cancel) const;

        qint32 collectFile(const QFileInfo& fileEntry, const QDir& rootDir, PboNode& rootNode) const;
    };
}
