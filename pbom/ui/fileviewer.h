#pragma once

#include <QString>
#include "exception.h"

namespace pboman3::ui {
    class FileViewerException : public AppException {
    public:
        FileViewerException(QString message, QString filePath);

        PBOMAN_EX_HEADER(FileViewerException)

        [[nodiscard]] const QString& filePath() const;

    private:
        QString filePath_;
    };

    class FileViewer {
    public:
        virtual void previewFile(const QString& path) = 0;

        virtual ~FileViewer() = default;
    };
}
