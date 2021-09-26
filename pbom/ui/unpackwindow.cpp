#include "unpackwindow.h"
#include <QFileDialog>
#include "model/task/unpackwindowmodel.h"

namespace pboman3 {
    UnpackWindow::UnpackWindow(QWidget* parent)
        : TaskWindow(parent) {
        QString title = "Unpack PBO(s) - ";
        title.append(PBOM_PROJECT_NAME);
        setWindowTitle(title);
    }

    void UnpackWindow::unpackFilesToTargetPath(const QStringList& files, const QString& targetPath) {
        const QSharedPointer<TaskWindowModel> model(new UnpackWindowModel(files, targetPath));
        show();
        start(model);
    }

    bool UnpackWindow::tryUnpackFilesWithPrompt(const QStringList& files) {
        const QString dir = QFileDialog::getExistingDirectory(this, "Directory to unpack into");
        if (dir.isEmpty())
            return false;

        unpackFilesToTargetPath(files, dir);
        return true;
    }
}
