#pragma once

#include <QString>

namespace pboman3 {
    class FileViewer {
    public:
        virtual void previewFile(const QString& path) = 0;

        virtual ~FileViewer() = default;
    };
}
