#include <QApplication>
#include "ui/mainwindow.h"

int main(int argc, char* argv[]) {
    auto model = QSharedPointer<PboModel>(new PboModel());
    QApplication a(argc, argv);

    if (argc > 1) {
        const QString file(argv[1]);
        if (QFile::exists(file)) {
            model->loadFile(file);
        }
    }

    MainWindow w(nullptr, model.get());
    w.show();

    return a.exec();
}
