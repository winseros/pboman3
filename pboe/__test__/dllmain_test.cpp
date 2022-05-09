#include <gtest/gtest.h>
#include <filesystem>
#include "dllmain_test.h"
#include "registry.h"

namespace pboman3::test {
    // TEST(DrawIconTest, IconDraws) {
    //     const string exePath = Registry::getExecutablePath();
    //
    //     HICON icon;
    //     ExtractIconEx(exePath.data(), 0, NULL, &icon, 1);
    //
    //     HDC dc = GetDC(NULL);
    //     HBITMAP bitmap = CreateCompatibleBitmap(dc, 16, 16);
    //     HGDIOBJ prev = SelectObject(dc, bitmap);
    //
    //     BOOL ok = DrawIcon(dc, 0, 0, icon);
    //
    //     DestroyIcon(icon);
    //     DeleteObject(bitmap);
    //     DeleteDC(dc);
    // }

    //TEST(DllRegisterServerTest, RegistersDevelopmentApplication) {
    //    using namespace std::filesystem;

    //    path binaryDir(BINARY_DIR);
    //    binaryDir = binaryDir.lexically_normal();
    //    const wstring exe = binaryDir.parent_path().append("pbom").append("pbom.exe").wstring();
    //    const wstring dll = binaryDir.append("pboe.dll").wstring();

    //    const HRESULT hr = Registry::registerServer(exe, dll);
    //    ASSERT_TRUE(SUCCEEDED(hr));
    //}

    TEST(DllRegisterServerTest, UnregistersDevelopmentApplication) {
        const HRESULT hr = Registry::unregisterServer();
        ASSERT_TRUE(SUCCEEDED(hr));
    }
}
