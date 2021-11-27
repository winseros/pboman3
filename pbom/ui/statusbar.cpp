#include "statusbar.h"
#include "exception.h"
#include "util/log.h"
#include "ui/win32/win32taskbarindicator.h"

namespace pboman3::ui {
    StatusBar::StatusBar(QWidget* parent)
        : QStatusBar(parent),
          progress_(new QProgressBar(this)),
          button_(new QPushButton(this)) {

        progress_->setTextVisible(false);
        progress_->setMinimum(0);
        progress_->setMaximum(0);
        progress_->setFixedHeight(15);
        progress_->setVisible(false);
        addWidget(progress_, 1); //ownership transferred!

        button_->setText("&Cancel");
        button_->setStyleSheet("padding: 0 5; alignment: center;");
        button_->setVisible(false);
        connect(button_, &QPushButton::released, [this]() { future_.cancel(); });
        addWidget(button_); //ownership transferred!
    }

    void StatusBar::progressShow(QFuture<void> future, bool supportsCancellation) {
        if (progress_->isVisible())
            throw InvalidOperationException("A new background operation must not begin while the previous is running");

        future_ = std::move(future);
        progress_->setVisible(true);
        if (supportsCancellation)
            button_->setVisible(true);

        taskbar_ = QSharedPointer<TaskbarIndicator>(new Win32TaskbarIndicator(effectiveWinId()));
        taskbar_->setIndeterminate();
    }

    void StatusBar::progressHide() {
        if (!progress_->isVisible())
            throw InvalidOperationException("There is no background operation in progress");

        progress_->setVisible(false);
        button_->setVisible(false);
        taskbar_.clear();
    }
}
