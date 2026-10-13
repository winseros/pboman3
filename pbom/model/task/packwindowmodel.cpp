#include "packwindowmodel.h"
#include "packtask.h"
#include <QFileInfo>

namespace pboman3::model::task {
    PackWindowModel::PackWindowModel(const QStringList& folders, const QString& outputDir, const bool besideInput,
                                     FileConflictResolutionMode::Enum fileConflictResolutionMode) {
        for (const QString& folder : folders) {
            const QString targetDir = besideInput ? QFileInfo(folder).absolutePath() : outputDir;
            QSharedPointer<Task> task(new PackTask(folder, targetDir, fileConflictResolutionMode));
            addTask(task);
        }
    }
}
