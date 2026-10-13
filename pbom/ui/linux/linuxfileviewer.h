#pragma once

#include "ui/fileviewer.h"

namespace pboman3::ui {
    class LinuxFileViewer final : public FileViewer {
    public:
        void previewFile(const QString& path) override;
    };
}
