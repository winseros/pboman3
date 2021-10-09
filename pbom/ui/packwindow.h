#pragma once

#include "taskwindow.h"

namespace pboman3 {
    class PackWindow : public TaskWindow {
    public:
        PackWindow(QWidget* parent);

        void packFoldersToOutputDir(const QStringList& folders, const QString& outputDir);

        bool tryPackFoldersWithPrompt(const QStringList& folders);
    };
}
