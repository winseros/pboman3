#pragma once

#include "taskwindow.h"

namespace pboman3 {
    class UnpackWindow : public TaskWindow {
    public:
        UnpackWindow(QWidget* parent);

        void unpackFilesToOutputDir(const QStringList& files, const QString& outputDir);

        bool tryUnpackFilesWithPrompt(const QStringList& files);
    };
}
