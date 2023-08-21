#pragma once

#include <Windows.h>
#include <processenv.h>
#include <shellapi.h>

namespace pboman3 {
    class Argv16Bit {
    public:
        wchar_t** argv;
        int argc;

        Argv16Bit() {
            const LPTSTR cmd = GetCommandLine();
            argv = CommandLineToArgvW(cmd, &argc);
        }

        Argv16Bit(const Argv16Bit&) = delete;

        Argv16Bit(Argv16Bit&&) = delete;

        ~Argv16Bit() {
            LocalFree(argv);
        }
    };
}
