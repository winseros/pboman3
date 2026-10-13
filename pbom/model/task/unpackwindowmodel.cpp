#include "unpackwindowmodel.h"
#include "unpacktask.h"
#include <QFileInfo>

namespace pboman3::model::task {
    UnpackWindowModel::UnpackWindowModel(const QStringList& pboFiles, const QString& outputDir, const bool besideInput,
                                         const bool usePboPrefix,
                                         FileConflictResolutionMode::Enum fileConflictResolutionMode) {
        for (const QString& pboFile : pboFiles) {
            const QString targetDir = besideInput ? QFileInfo(pboFile).absolutePath() : outputDir;
            QSharedPointer<Task> task(new UnpackTask(pboFile, targetDir, usePboPrefix, fileConflictResolutionMode));
            addTask(task);
        }
    }
}
