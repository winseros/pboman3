#include "busybar.h"

namespace pboman3 {
    BusyBar::BusyBar(QWidget* tree)
        : QProgressBar(),
          count_(0),
          tree_(tree) {
        setTextVisible(false);
        setMinimum(0);
        setMaximum(0);
        setFixedHeight(15);
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
