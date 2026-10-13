#include "linuxtaskbarindicator.h"

#include <QApplication>
#include <QWidget>

namespace pboman3::ui {
    LinuxTaskbarIndicator::LinuxTaskbarIndicator(QWidget* window)
        : window_(window),
          isError_(false) {
    }

    LinuxTaskbarIndicator::~LinuxTaskbarIndicator() {
        if (!isError_ && window_ && !window_->isActiveWindow())
            QApplication::alert(window_);
    }

    void LinuxTaskbarIndicator::setProgressValue(qint64, qint64) {
        isError_ = false;
    }

    void LinuxTaskbarIndicator::setIndeterminate() {
        isError_ = false;
    }

    void LinuxTaskbarIndicator::setError() {
        isError_ = true;
    }
}
