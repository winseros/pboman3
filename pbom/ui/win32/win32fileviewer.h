#pragma once

#include "ui/fileviewer.h"
#include "exception.h"

namespace pboman3::ui {
    class Win32FileViewerException : public AppException {
    public:
        Win32FileViewerException(QString message);

        PBOMAN_EX_HEADER(Win32FileViewerException)
    };

    class Win32FileViewer : public FileViewer {
    public:
        void previewFile(const QString& path) override;
    };
}
