#include "win32taskbarindicator.h"
#include <Windows.h>

namespace pboman3::ui {
    Win32TaskbarIndicator::Win32TaskbarIndicator(WId windowId)
        : window_(reinterpret_cast<HWND>(windowId)), // NOLINT(performance-no-int-to-ptr)
          isErr_(false) {
        const HRESULT hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_ITaskbarList3,
                                            reinterpret_cast<LPVOID*>(&progress_));
        if (!SUCCEEDED(hr))
            progress_ = 0;
    }

    Win32TaskbarIndicator::~Win32TaskbarIndicator() {
        if (progress_) {
            progress_->SetProgressState(window_, TBPF_NOPROGRESS);
            progress_->Release();
            if (!isErr_ && !windowHasFocus())
                flashWindow();
        }
    }

    void Win32TaskbarIndicator::setProgressValue(qint64 value, qint64 maxValue) {
        if (!progress_)
            return;
        isErr_ = false;
        progress_->SetProgressState(window_, TBPF_NORMAL);
        progress_->SetProgressValue(window_, value, maxValue);
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
}
