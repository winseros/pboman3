#include <QApplication>
#include "ui/mainwindow.h"

int main(int argc, char* argv[]) {
    using namespace pboman3;

    const auto model = QSharedPointer<PboModel>(new PboModel());
    QApplication a(argc, argv);

    MainWindow w(nullptr, model.get());
    w.show();

    if (argc > 1) {
        const QString file(argv[1]);
        if (QFile::exists(file)) {
            w.loadFile(file);
        }
    }

    return a.exec();
}
