#pragma once

#include "ui/fileviewer.h"
#include "util/exception.h"

namespace pboman3::ui {
    class Win32FileViewerException : public AppException {
    public:
        Win32FileViewerException(QString message);

        friend QDebug operator<<(QDebug debug, const Win32FileViewerException& ex);

        QException* clone() const override;
    };

    class Win32FileViewer : public FileViewer {
    public:
        void previewFile(const QString& path) override;
    };
}
