#pragma once

#include <ShObjIdl.h>
#include <qwindowdefs.h>
#include "ui/taskbarindicator.h"

namespace pboman3::ui {
    class Win32TaskbarIndicator : public TaskbarIndicator {
    public:
        explicit Win32TaskbarIndicator(WId windowId);

        ~Win32TaskbarIndicator() override;

        void setProgressValue(qint64 value, qint64 maxValue) override;

        void setIndeterminate() override;

        void setError() override;

    private:
        ITaskbarList3* progress_;
        HWND window_;
        int progressValue_;
        bool isErr_;

        [[nodiscard]] bool windowHasFocus() const;

        void flashWindow() const;

        static const int PROGRESS_VALUE_INITIAL;
    };
}
