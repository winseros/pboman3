#pragma once

#include <Windows.h>

namespace pboman3 {
    class StringBuilder {
    public:
        StringBuilder(size_t bufSize);

        ~StringBuilder();

        StringBuilder& append(LPCTSTR str);

        LPTSTR value;

    private:
        size_t bufSize_;
        size_t dataSize_;
    };
}
