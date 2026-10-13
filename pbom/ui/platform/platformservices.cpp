#include "platformservices.h"

#include <QWidget>

#ifdef Q_OS_WIN
#include "ui/win32/win32fileviewer.h"
#include "ui/win32/win32iconmgr.h"
#include "ui/win32/win32taskbarindicator.h"
#else
#include "ui/linux/linuxfileviewer.h"
#include "ui/linux/linuxiconmgr.h"
#include "ui/linux/linuxtaskbarindicator.h"
#endif

namespace pboman3::ui {
    QSharedPointer<IconMgr> CreateIconMgr() {
#ifdef Q_OS_WIN
        return QSharedPointer<Win32IconMgr>::create();
#else
        return QSharedPointer<LinuxIconMgr>::create();
#endif
    }

    QSharedPointer<FileViewer> CreateFileViewer() {
#ifdef Q_OS_WIN
        return QSharedPointer<Win32FileViewer>::create();
#else
        return QSharedPointer<LinuxFileViewer>::create();
#endif
    }

    QSharedPointer<TaskbarIndicator> CreateTaskbarIndicator(QWidget* window) {
#ifdef Q_OS_WIN
        return QSharedPointer<Win32TaskbarIndicator>::create(window->effectiveWinId());
#else
        return QSharedPointer<LinuxTaskbarIndicator>::create(window);
#endif
    }
}
