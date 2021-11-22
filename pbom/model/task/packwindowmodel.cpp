#include "packwindowmodel.h"
#include "packtask.h"

namespace pboman3::model {
    PackWindowModel::PackWindowModel(const QStringList& folders, const QString& outputDir) {
        for (const QString& folder : folders) {
            QSharedPointer<Task> task(new PackTask(folder, outputDir));
            addTask(task);
        }
    }
}
