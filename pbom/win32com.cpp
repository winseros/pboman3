#include "win32com.h"
#include "util/log.h"
#include <QTimer>
#include <Windows.h>

#define LOG(...) LOGGER("Win32Com", __VA_ARGS__)

namespace pboman3 {
    Win32Com::Win32Com(const QApplication* app)
        : initialized_(false) {
        assert(app);

        auto* timer = new QTimer(new QTimer());
        timer->moveToThread(app->thread());
        timer->setSingleShot(true);
        QObject::connect(timer, &QTimer::timeout, [timer, this]() {
            LOG(info, "Initializing COM")

            const HRESULT hr = CoInitialize(nullptr);

            LOG(info, "COM init status:", hr)

            assert(SUCCEEDED(hr));
            timer->deleteLater();

            initialized_.store(true);
        });
        timer->start();

        QObject::connect(app, &QCoreApplication::aboutToQuit, [this]() {
            if (initialized_) {
                LOG(info, "Uninitializing COM")
                CoUninitialize();
            }
        });
    }
}
