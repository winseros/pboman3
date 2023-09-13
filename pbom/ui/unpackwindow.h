#pragma once

#include "taskwindow.h"
#include "model/task/unpackwindowmodel.h"

namespace pboman3::ui {
    class UnpackWindow : public TaskWindow {
    public:
        UnpackWindow(QWidget* parent, UnpackWindowModel* model);

        static bool tryRequestTargetFolder(QString& output);
    };
}
