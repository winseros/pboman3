#include "busybar.h"

namespace pboman3 {
    BusyBar::BusyBar(QStatusBar* host, QWidget* tree)
        : QProgressBar(),
          count_(0),
          tree_(tree) {
        setTextVisible(false);
        setMinimum(0);
        setMaximum(0);
        setFixedHeight(15);
        host->addWidget(this, 1);
        hide();
    }

    void BusyBar::start() {
        if (count_ == 0) {
            show();
            tree_->setDisabled(true);
        }
        count_++;
    }

    void BusyBar::stop() {
        count_--;
        if (count_ == 0) {
            hide();
            tree_->setEnabled(true);
            tree_->setFocus();
        }
    }
}
