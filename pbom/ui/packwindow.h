#pragma once

#include "taskwindow.h"
#include "model/task/packwindowmodel.h"

namespace pboman3::ui {
    class PackWindow : public TaskWindow {
    public:
        PackWindow(QWidget* parent, PackWindowModel* model);

        static bool tryRequestTargetFolder(QString& output);
    };
}
