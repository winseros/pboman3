#pragma once

#include <QApplication>
#include <atomic>

namespace pboman3 {
    class Win32Com {
    public:
        Win32Com(const QApplication* app);

    private:
        std::atomic_bool initialized_;
    };
}
