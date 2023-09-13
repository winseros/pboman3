#include "unpackwindow.h"
#include <QFileDialog>
#include "model/task/unpackwindowmodel.h"

namespace pboman3::ui {
    UnpackWindow::UnpackWindow(QWidget* parent, UnpackWindowModel* model)
        : TaskWindow(parent, model) {
        QString title = "Unpack PBO(s) - ";
        title.append(PBOM_PROJECT_NAME);
        setWindowTitle(title);
    }

    bool UnpackWindow::tryRequestTargetFolder(QString& output) {
        output = QFileDialog::getExistingDirectory(nullptr, "Directory to unpack into");
        return !output.isEmpty();
    }
}
