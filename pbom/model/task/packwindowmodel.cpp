#include "packwindowmodel.h"
#include "packtask.h"

namespace pboman3::model::task {
    PackWindowModel::PackWindowModel(const QStringList& folders, const QString& outputDir,
                                     FileConflictResolutionMode::Enum fileConflictResolutionMode) {
        for (const QString& folder : folders) {
            QSharedPointer<Task> task(new PackTask(folder, outputDir, fileConflictResolutionMode));
            addTask(task);
        }
    }
}
