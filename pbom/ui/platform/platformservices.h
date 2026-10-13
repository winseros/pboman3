#pragma once

#include <QSharedPointer>

class QWidget;

namespace pboman3::ui {
    class FileViewer;
    class IconMgr;
    class TaskbarIndicator;

    [[nodiscard]] QSharedPointer<IconMgr> CreateIconMgr();

    [[nodiscard]] QSharedPointer<FileViewer> CreateFileViewer();

    [[nodiscard]] QSharedPointer<TaskbarIndicator> CreateTaskbarIndicator(QWidget* window);
}
