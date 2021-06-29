#include "statusbar.h"
#include "util/appexception.h"
#include "util/log.h"

#define LOG(...) LOGGER("ui/StatusBar", __VA_ARGS__)

namespace pboman3 {
    StatusBar::StatusBar(QWidget* parent)
        : QStatusBar(parent) {

        progress_ = new QProgressBar(this);
        progress_->setTextVisible(false);
        progress_->setMinimum(0);
        progress_->setMaximum(0);
        progress_->setFixedHeight(15);
        progress_->setVisible(false);
        addWidget(progress_, 1); //ownership transferred!

        button_ = new QPushButton(this);
        button_->setText("&Cancel");
        button_->setStyleSheet("padding: 0 5; alignment: center;");
        button_->setVisible(false);
        connect(button_, &QPushButton::released, [this]() { future_.cancel(); });
        addWidget(button_); //ownership transferred!
    }

    void StatusBar::progressShow(QFuture<void> future) {
        if (progress_->isVisible())
            throw AppException("A new background operation must not begin while the previous is running");

        future_ = future;
        progress_->setVisible(true);
        button_->setVisible(true);
    }

    void StatusBar::progressHide() {
        progress_->setVisible(false);
        button_->setVisible(false);
    }
}
