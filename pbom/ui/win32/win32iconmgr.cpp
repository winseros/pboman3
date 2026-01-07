#include "win32iconmgr.h"
#include <QPixmap>
#include <QImage>
#include "exception.h"
#include <Windows.h>
#include <shellapi.h>
#include "util/log.h"

#define LOG(...) LOGGER("ui/win32/Win32IconMgr", __VA_ARGS__)

namespace pboman3::ui {
    Win32IconMgr::Win32IconMgr() {
        cache_[""] = QIcon(":ifile.png");
        cache_[":folder-closed:"] = QIcon(":ifolderclosed.png");
        cache_[":folder-opened:"] = QIcon(":ifolderopened.png");
    }

    const QIcon& Win32IconMgr::getIconForExtension(const QString& extension) {
        if (extension.startsWith("."))
            throw AppException("The extension must not start with a \".\" symbol");

        if (cache_.contains(extension)) {
            return cache_[extension];
        }

        LOG(debug, "Get icon for extension:", extension)

        SHFILEINFOW info;
        const QString fn = "file." + extension;
        const DWORD_PTR hr = SHGetFileInfoW(
            reinterpret_cast<LPCWSTR>(fn.utf16()),
            FILE_ATTRIBUTE_NORMAL,
            &info,
            sizeof info,
            SHGFI_ICON | SHGFI_USEFILEATTRIBUTES);

        LOG(debug, "Retrieve from the OS:", hr)

        if (SUCCEEDED(hr)) {
            cache_[extension] = QIcon(QPixmap::fromImage(QImage::fromHICON(info.hIcon)));
            DestroyIcon(info.hIcon);
            return cache_[extension];
        }

        LOG(warning, "Retrieve failed - fall back to the default", hr)

        return cache_[""];
    }

    const QIcon& Win32IconMgr::getFolderOpenedIcon() {
        return cache_[":folder-opened:"];
    }

    const QIcon& Win32IconMgr::getFolderClosedIcon() {
        return cache_[":folder-closed:"];
    }
}
