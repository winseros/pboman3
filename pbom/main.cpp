#include <QApplication>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    auto model = QSharedPointer<PboModel2>(new PboModel2());
    QApplication a(argc, argv);
    MainWindow w(nullptr, model.get());
    w.show();

    if (argc > 1) {
        const QString file(argv[1]);
        if (QFile::exists(file)) {
            model->loadFile(file);
        }
    }

    return a.exec();
}
