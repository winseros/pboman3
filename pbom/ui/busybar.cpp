#include "busybar.h"
#include "util/log.h"

#define LOG(...) LOGGER("ui/BusyBar", __VA_ARGS__)

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
            LOG(info, "Show the control")
            show();
            tree_->setDisabled(true);
        }
        count_++;
        LOG(info, "The counter was increased to:", count_)
    }

    void BusyBar::stop() {
        count_--;
        LOG(info, "The counter was decreased to:", count_)

        if (count_ == 0) {
            LOG(info, "Hide the control")
            hide();
            tree_->setEnabled(true);
            tree_->setFocus();
        }
    }
}
