#include "fileviewer.h"

#include <QDebug>

namespace pboman3::ui {
    FileViewerException::FileViewerException(QString message, QString filePath)
        : AppException(std::move(message)),
          filePath_(std::move(filePath)) {
    }

    const QString& FileViewerException::filePath() const {
        return filePath_;
    }

    PBOMAN_EX_IMPL_DEFAULT(FileViewerException)
}
