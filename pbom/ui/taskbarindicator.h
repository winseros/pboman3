#pragma once

namespace pboman3::ui {
    class TaskbarIndicator {
    public:
        virtual ~TaskbarIndicator() = default;

        virtual void setProgressValue(qint64 value, qint64 maxValue) = 0;

        virtual void setIndeterminate() = 0;

        virtual void setError() = 0;
    };
}
