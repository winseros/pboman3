#include "win32fileviewer.h"
#include <QVariant>
#include <Windows.h>
#include <shellapi.h>
#include "util/log.h"

#define LOG(...) LOGGER("ui/win32/Win32FileViewer", __VA_ARGS__)

namespace pboman3::ui {
    Win32FileViewerException::Win32FileViewerException(QString message, unsigned long systemErrorCode,
                                                       QString systemErrorDescription, QString filePath)
        : AppException(std::move(message)),
          systemErrorCode_(systemErrorCode),
          systemErrorDescription_(std::move(systemErrorDescription)),
          filePath_(std::move(filePath)) {
    }

    PBOMAN_EX_IMPL_DEFAULT(Win32FileViewerException)

    void Win32FileViewer::previewFile(const QString& path) {
        LOG(info, "Launching preview for:", path)

        SHELLEXECUTEINFOW info;
        info.fMask = SEE_MASK_DEFAULT;
        info.lpVerb = nullptr;
        info.lpFile = reinterpret_cast<LPCWSTR>(path.utf16());
        info.lpParameters = nullptr;
        info.lpDirectory = nullptr;
        info.nShow = SW_NORMAL;
        info.cbSize = sizeof info;
        if (!ShellExecuteExW(&info)) {
            const DWORD err = GetLastError();
            LOG(warning, "Preview failed with error:", err)

            QByteArray message(1024, Qt::Initialization::Uninitialized);
            const qsizetype messageSize = FormatMessage(
                FORMAT_MESSAGE_FROM_SYSTEM, nullptr, err, 0,
                reinterpret_cast<LPWSTR>(message.data()), message.length(),
                nullptr);

            auto str = QString::fromUtf16(reinterpret_cast<char16_t*>(message.data()), messageSize);

            throw Win32FileViewerException(
                "The underlying operating system returned error trying to open the file.", err, std::move(str), path);
        }
    }
}
