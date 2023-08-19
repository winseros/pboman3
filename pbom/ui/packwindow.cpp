#include "packwindow.h"
#include <QFileDialog>
#include "model/task/packwindowmodel.h"

namespace pboman3::ui {
    using namespace model;

    PackWindow::PackWindow(QWidget* parent, PackWindowModel* model)
        : TaskWindow(parent, model) {
        QString title = "Pack PBO(s) - ";
        title.append(PBOM_PROJECT_NAME);
        setWindowTitle(title);
    }

    bool PackWindow::tryRequestTargetFolder(QString& output) {
        output = QFileDialog::getExistingDirectory(nullptr, "Directory to pack into");
        return !output.isEmpty();
    }
}
