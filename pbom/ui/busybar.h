#pragma once

#include <QProgressBar>
#include <QStatusBar>

namespace pboman3 {
    class BusyBar : public QProgressBar {
    public:
        BusyBar(QWidget* tree);

        void start();

        void stop();
    private:
        int count_;
        QWidget* tree_;
    };
}
