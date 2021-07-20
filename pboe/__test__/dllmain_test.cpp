#include <gtest/gtest.h>
#include "dllmain_test.h"

#include "registry.h"

namespace pboman3::test {
    TEST(DllRegisterServerTest, RegistersEntryInRegistry) {
        const HRESULT hr = DllRegisterServer();
        ASSERT_TRUE(SUCCEEDED(hr));
    }

    TEST(DllUnregisterServerTest, UnregistersEntryInRegistry) {
        const HRESULT hr = DllUnregisterServer();
        ASSERT_TRUE(SUCCEEDED(hr));
    }

    TEST(DrawIconTest, IconDraws) {
        const string exePath = Registry::getExecutablePath();

        HICON icon;
        ExtractIconEx(exePath.data(), 0, NULL, &icon, 1);

        HDC dc = GetDC(NULL);
        HBITMAP bitmap = CreateCompatibleBitmap(dc, 16, 16);
        HGDIOBJ prev = SelectObject(dc, bitmap);

        BOOL ok = DrawIcon(dc, 0, 0, icon);

        DestroyIcon(icon);
        DeleteObject(bitmap);
        DeleteDC(dc);
    }
}
