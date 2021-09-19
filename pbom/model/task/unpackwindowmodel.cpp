#include "unpackwindowmodel.h"
#include "unpacktask.h"

namespace pboman3 {
    UnpackWindowModel::UnpackWindowModel(const QStringList& pboFiles, const QString& targetPath) {
        for (const QString& pboFile : pboFiles) {
            QSharedPointer<Task> task(new UnpackTask(pboFile, targetPath));
            addTask(task);
        }
    }
}
