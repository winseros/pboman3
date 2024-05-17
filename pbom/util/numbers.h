#pragma once

#include <concepts>

namespace pboman3::util {
    class NumberUtils {
    public:
        template<std::integral T>
        [[nodiscard]] static int GetNumberOfDigits(T number) {
            int res = 1;
            while (number >= 10) {
                number = number * 0.1;
                res++;
            }
            return res;
        }
    };
}