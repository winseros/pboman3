#include "win32fileviewer.h"
#include <QVariant>
#include <Windows.h>
#include <shellapi.h>
#include "util/log.h"

#define LOG(...) LOGGER("ui/win32/Win32FileViewer", __VA_ARGS__)

namespace pboman3 {
    Win32FileViewerException::Win32FileViewerException(QString message)
        : AppException(std::move(message)) {
    }

    QDebug operator<<(QDebug debug, const Win32FileViewerException& ex) {
        return debug << "Win32FileViewerException:" << ex.message();
    }

    QException* Win32FileViewerException::clone() const {
        return new Win32FileViewerException(*this);
    }

    void Win32FileViewer::previewFile(const QString& path) {
        LOG(info, "Launching preview for:", path)

        const QByteArray pathData = path.toUtf8();

        SHELLEXECUTEINFOA info;
        info.fMask = SEE_MASK_DEFAULT;
        info.lpVerb = nullptr;
        info.lpFile = pathData.constData();
        info.lpParameters = nullptr;
        info.lpDirectory = nullptr;
        info.nShow = SW_NORMAL;
        info.cbSize = sizeof info;
        if (!ShellExecuteExA(&info)) {
            const DWORD err = GetLastError();
            LOG(warning, "Preview failed with error:", err)
            throw Win32FileViewerException(
                "The underlying operating system returned error trying to open the file. Error code: " + QString::number(err));
        }
    }
}
