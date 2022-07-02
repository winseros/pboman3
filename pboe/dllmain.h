#pragma once

#define PBOM_SHELL_CLSID L"{dd2a27fa-7c7f-4b50-9b54-836af42fb64d}"
#define PBOM_SHELL_PROGID L"pboman3_pbo"
#define __W(T) L##T
#define W(T) __W(T)


namespace pboman3 {
    void DllAddRef();

    void DllRelease();
}
