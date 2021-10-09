#include "packwindow.h"
#include <QFileDialog>
#include "model/task/packwindowmodel.h"

namespace pboman3 {
    PackWindow::PackWindow(QWidget* parent)
        : TaskWindow(parent) {
        QString title = "Pack PBO(s) - ";
        title.append(PBOM_PROJECT_NAME);
        setWindowTitle(title);
    }

    void PackWindow::packFoldersToOutputDir(const QStringList& folders, const QString& outputDir) {
        const QSharedPointer<TaskWindowModel> model(new PackWindowModel(folders, outputDir));
        show();
        start(model);
    }

    bool PackWindow::tryPackFoldersWithPrompt(const QStringList& folders) {
        const QString dir = QFileDialog::getExistingDirectory(this, "Directory to pack into");
        if (dir.isEmpty())
            return false;

        packFoldersToOutputDir(folders, dir);
        return true;
    }
}
