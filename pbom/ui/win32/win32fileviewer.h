#pragma once

#include "ui/fileviewer.h"
#include "exception.h"

namespace pboman3::ui {
    class Win32FileViewerException : public AppException {
    public:
        Win32FileViewerException(QString message, unsigned long systemErrorCode, QString systemErrorDescription,
                                 QString filePath);

        PBOMAN_EX_HEADER(Win32FileViewerException)

        [[nodiscard]] unsigned long systemErrorCode() const {
            return systemErrorCode_;
        }

        [[nodiscard]] QString systemErrorDescription() const {
            return systemErrorDescription_;
        }

        [[nodiscard]] QString filePath() const {
            return filePath_;
        }

    private:
        unsigned long systemErrorCode_;
        QString systemErrorDescription_;
        QString filePath_;
    };

    class Win32FileViewer : public FileViewer {
    public:
        void previewFile(const QString& path) override;
    };
}
