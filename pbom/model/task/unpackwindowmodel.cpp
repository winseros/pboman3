#include "unpackwindowmodel.h"
#include "unpacktask.h"

namespace pboman3::model {
    UnpackWindowModel::UnpackWindowModel(const QStringList& pboFiles, const QString& outputDir) {
        for (const QString& pboFile : pboFiles) {
            QSharedPointer<Task> task(new UnpackTask(pboFile, outputDir));
            addTask(task);
        }
    }
}
