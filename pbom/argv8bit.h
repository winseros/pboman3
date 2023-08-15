#pragma once

#include "exception.h"

namespace pboman3 {
    class Argv8Bit {
    public:
        static char** acquire(int argc, wchar_t* argv[]) {
            if (argv8Bit_)
                throw AppException("This method is designed to be called only once");

            argv8Bit_ = new char*[argc];
            for (auto i = 0; i < argc; i++) {
                const auto argLen = std::wcslen(argv[i]);
                argv8Bit_[i] = static_cast<char*>(calloc(1, argLen + 1));
                assert(argv8Bit_[i] && "Allocation failed");
                for (size_t j = 0; j < argLen; j++) {
                    auto chr = argv[i][j];
                    if (chr > std::numeric_limits<char>::max()) {
                        chr = 'x';
                    }
                    argv8Bit_[i][j] = static_cast<char>(chr);
                }
            }
            return argv8Bit_;
        }

        static void release() {
            delete[] argv8Bit_;
        }

    private:
        inline static char** argv8Bit_ = nullptr;
    };
}
