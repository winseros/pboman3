#include "linuxiconmgr.h"

#include <QMimeDatabase>
#include "exception.h"
#include "util/log.h"

#define LOG(...) LOGGER("ui/linux/LinuxIconMgr", __VA_ARGS__)

namespace pboman3::ui {
    namespace {
        constexpr auto DEFAULT_ICON = ":default:";
        constexpr auto FOLDER_CLOSED_ICON = ":folder-closed:";
        constexpr auto FOLDER_OPENED_ICON = ":folder-opened:";

        QIcon themedIcon(const QString& name, const QString& genericName, const QIcon& fallback) {
            QIcon icon;
            if (!name.isEmpty())
                icon = QIcon::fromTheme(name);
            if (icon.isNull() && !genericName.isEmpty())
                icon = QIcon::fromTheme(genericName);
            return icon.isNull() ? fallback : icon;
        }
    }

    LinuxIconMgr::LinuxIconMgr() {
        cache_[DEFAULT_ICON] = QIcon(":ifile.png");
        cache_[FOLDER_CLOSED_ICON] = themedIcon("folder", {}, QIcon(":ifolderclosed.png"));
        cache_[FOLDER_OPENED_ICON] = themedIcon("folder-open", "folder", QIcon(":ifolderopened.png"));
    }

    const QIcon& LinuxIconMgr::getIconForExtension(const QString& extension) {
        if (extension.startsWith('.'))
            throw AppException("The extension must not start with a \".\" symbol");

        const QString normalizedExtension = extension.toLower();
        if (cache_.contains(normalizedExtension))
            return cache_[normalizedExtension];

        const QMimeType mimeType = QMimeDatabase().mimeTypeForFile(
            "file." + normalizedExtension, QMimeDatabase::MatchExtension);
        LOG(debug, "Resolved", normalizedExtension, "as", mimeType.name())

        cache_[normalizedExtension] = themedIcon(
            mimeType.iconName(), mimeType.genericIconName(), cache_[DEFAULT_ICON]);
        return cache_[normalizedExtension];
    }

    const QIcon& LinuxIconMgr::getFolderOpenedIcon() {
        return cache_[FOLDER_OPENED_ICON];
    }

    const QIcon& LinuxIconMgr::getFolderClosedIcon() {
        return cache_[FOLDER_CLOSED_ICON];
    }
}
