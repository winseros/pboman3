#pragma once

#include <QPointer>
#include "ui/taskbarindicator.h"

class QWidget;

namespace pboman3::ui {
    class LinuxTaskbarIndicator final : public TaskbarIndicator {
    public:
        explicit LinuxTaskbarIndicator(QWidget* window);

        ~LinuxTaskbarIndicator() override;

        void setProgressValue(qint64 value, qint64 maxValue) override;

        void setIndeterminate() override;

        void setError() override;

    private:
        QPointer<QWidget> window_;
        bool isError_;
    };
}
