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

    void UnpackWindow::unpackFilesToOutputDir(const QStringList& files, const QString& outputDir) {
        const QSharedPointer<TaskWindowModel> model(new UnpackWindowModel(files, outputDir));
        show();
        start(model);
    }

    bool UnpackWindow::tryUnpackFilesWithPrompt(const QStringList& files) {
        const QString dir = QFileDialog::getExistingDirectory(this, "Directory to unpack into");
        if (dir.isEmpty())
            return false;

        unpackFilesToOutputDir(files, dir);
        return true;
    }
}
