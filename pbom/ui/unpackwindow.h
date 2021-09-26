#pragma once

#include "taskwindow.h"

namespace pboman3 {
    class UnpackWindow : public TaskWindow {
    public:
        UnpackWindow(QWidget* parent);

        void unpackFilesToTargetPath(const QStringList& files, const QString& targetPath);

        bool tryUnpackFilesWithPrompt(const QStringList& files);
    };
}
