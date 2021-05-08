#pragma once
#include <QProgressBar>
#include <QStatusBar>

namespace pboman3 {
    class BusyBar : QProgressBar {
    public:
        BusyBar(QStatusBar* host, QWidget* tree);

        void start();

        void stop();
    private:
        int count_;
        QWidget* tree_;
    };
}
