#include "win32taskbarindicator.h"
#include <Windows.h>

namespace pboman3::ui {
    Win32TaskbarIndicator::Win32TaskbarIndicator(WId windowId)
        : window_(reinterpret_cast<HWND>(windowId)), // NOLINT(performance-no-int-to-ptr)
          progressValue_(PROGRESS_VALUE_INITIAL),
          isErr_(false) {
        const HRESULT hr = CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_INPROC_SERVER, IID_ITaskbarList3,
                                            reinterpret_cast<LPVOID*>(&progress_));
        if (!SUCCEEDED(hr))
            progress_ = nullptr;
    }

    Win32TaskbarIndicator::~Win32TaskbarIndicator() {
        if (progress_) {
            progress_->SetProgressState(window_, TBPF_NOPROGRESS);
            progress_->Release();
            if (!isErr_ && !windowHasFocus())
                flashWindow();
        }
    }

    void Win32TaskbarIndicator::setProgressValue(const qint64 value, const qint64 maxValue) {
        if (!progress_)
            return;

        static constexpr int PROGRESS_VALUE_MAX = 100;

        const auto progressValue = static_cast<int>(ceil(PROGRESS_VALUE_MAX * static_cast<double>(value) / static_cast<double>(maxValue)));
        if (progressValue == progressValue_)
            return;

        if (progressValue_ == PROGRESS_VALUE_INITIAL)
            progress_->SetProgressState(window_, TBPF_NORMAL);

        isErr_ = false;
        progress_->SetProgressValue(window_, progressValue, PROGRESS_VALUE_MAX);

        progressValue_ = progressValue;
    }

    void Win32TaskbarIndicator::setIndeterminate() {
        if (progress_) {
            isErr_ = false;
            progress_->SetProgressState(window_, TBPF_INDETERMINATE);
        }
    }

    void Win32TaskbarIndicator::setError() {
        if (progress_) {
            isErr_ = true;
            progress_->SetProgressState(window_, TBPF_ERROR);
        }
    }

    bool Win32TaskbarIndicator::windowHasFocus() const {
        return GetForegroundWindow() == window_;
    }

    void Win32TaskbarIndicator::flashWindow() const {
        FLASHWINFO fi;
        fi.cbSize = sizeof fi;
        fi.hwnd = window_;
        fi.dwFlags = FLASHW_TRAY | FLASHW_TIMERNOFG;
        fi.dwTimeout = 0;
        FlashWindowEx(&fi);
    }

    const int Win32TaskbarIndicator::PROGRESS_VALUE_INITIAL = -1;
}
