#include "linuxfileviewer.h"

#include <QDesktopServices>
#include <QUrl>
#include "util/log.h"

#define LOG(...) LOGGER("ui/linux/LinuxFileViewer", __VA_ARGS__)

namespace pboman3::ui {
    void LinuxFileViewer::previewFile(const QString& path) {
        LOG(info, "Launching preview for:", path)
        if (!QDesktopServices::openUrl(QUrl::fromLocalFile(path))) {
            LOG(warning, "The desktop rejected the preview request for:", path)
            throw FileViewerException(
                "The desktop could not open the file with its associated application.", path);
        }
    }
}
