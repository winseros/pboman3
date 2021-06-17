#include <QApplication>
#include <QFile>
#include <QTimer>
#include <Windows.h>
#include "ui/mainwindow.h"

int main(int argc, char* argv[]) {
    using namespace pboman3;

    const auto model = QSharedPointer<PboModel>(new PboModel());
    QApplication a(argc, argv);

    auto* timer = new QTimer(new QTimer());
    timer->moveToThread(a.thread());
    timer->setSingleShot(true);
    QObject::connect(timer, &QTimer::timeout, [timer]() {
        const HRESULT hr = CoInitialize(nullptr);
        assert(SUCCEEDED(hr));
        timer->deleteLater();
    });
    timer->start();

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
