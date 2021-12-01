#pragma once

#include <QString>

namespace pboman3::ui {
    class FileViewer {
    public:
        virtual void previewFile(const QString& path) = 0;

        virtual ~FileViewer() = default;
    };
}
