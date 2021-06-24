#include <QApplication>
#include <QFile>
#include <QTimer>
#include <Windows.h>
#include "ui/errordialog.h"
#include "ui/mainwindow.h"
#include "util/appexception.h"
#include "util/log.h"

#define LOG(...) LOGGER("Main", __VA_ARGS__)

namespace pboman3 {
    class PboApplication : public QApplication {
    public:
        PboApplication(int& argc, char** argv)
            : QApplication(argc, argv) {
        }

        bool notify(QObject* o, QEvent* e) override {
            try {
                return QApplication::notify(o, e);
            } catch (const AppException& ex) {
                ErrorDialog(ex.message(), activeWindow()).exec();
                return true;
            }
        }
    };
}

int main(int argc, char* argv[]) {
    qSetMessagePattern(
        "%{time yyyy-MM-dd HH:mm:ss.zzz}|%{if-debug}DBG%{endif}%{if-info}INF%{endif}%{if-warning}WRN%{endif}%{if-critical}CRT%{endif}%{if-fatal}FTL%{endif}|%{file}|%{message}");

    using namespace pboman3;

    LOG(info, "Starting the app")

    const auto model = QSharedPointer<PboModel>(new PboModel());
    PboApplication a(argc, argv);

    auto* timer = new QTimer(new QTimer());
    timer->moveToThread(a.thread());
    timer->setSingleShot(true);
    QObject::connect(timer, &QTimer::timeout, [timer]() {
        LOG(info, "Initializing COM")

        const HRESULT hr = CoInitialize(nullptr);

        LOG(info, "COM init status:", hr)

        assert(SUCCEEDED(hr));
        timer->deleteLater();
    });
    timer->start();

    LOG(info, "Display the main window")
    MainWindow w(nullptr, model.get());
    w.show();

    LOG(info, "Number of startup args:", argc)

    if (argc > 1) {
        const QString file(argv[1]);
        if (QFile::exists(file)) {
            LOG(info, "Loading the file:", file);
            w.loadFile(file);
        } else {
            LOG(info, "The arg file did not exist:", file);
        }
    }

    const int exitCode = a.exec();

    LOG(info, "The app exiting with the code:", exitCode);

    return exitCode;
}
