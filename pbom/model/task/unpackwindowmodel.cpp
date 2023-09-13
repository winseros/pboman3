#include "unpackwindowmodel.h"
#include "unpacktask.h"

namespace pboman3::model::task {
    UnpackWindowModel::UnpackWindowModel(const QStringList& pboFiles, const QString& outputDir,
                                         FileConflictResolutionMode::Enum fileConflictResolutionMode) {
        for (const QString& pboFile : pboFiles) {
            QSharedPointer<Task> task(new UnpackTask(pboFile, outputDir, fileConflictResolutionMode));
            addTask(task);
        }
    }
}
