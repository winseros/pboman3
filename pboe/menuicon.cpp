#include "menuicon.h"

#include <cassert>

namespace pboman3 {
    MenuIcon::MenuIcon(LPCSTR exePath)
        : icon_(nullptr) {
        HICON icon;
        if (ExtractIconEx(exePath, 0, &icon, NULL, 1)) {
            // ReSharper disable CppLocalVariableMayBeConst
            HDC hdc = CreateDC("DISPLAY", NULL, NULL, NULL);

            HDC dc = CreateCompatibleDC(hdc);

            const int cx = GetSystemMetrics(SM_CXSMICON);
            const int cy = GetSystemMetrics(SM_CYSMICON);

            HBITMAP bitmap = CreateCompatibleBitmap(hdc, cx, cy);
            HGDIOBJ prev = SelectObject(dc, bitmap);

            HBRUSH brush = GetSysColorBrush(COLOR_MENU);
            RECT rect{0, 0, cx, cy};
            FillRect(dc, &rect, brush);

            DrawIconEx(dc, 0, 0, icon, cx, cy, 0, NULL, DI_NORMAL);

            HGDIOBJ btmp = SelectObject(dc, prev);
            assert(btmp == bitmap);
            // ReSharper restore CppLocalVariableMayBeConst

            DestroyIcon(icon);
            DeleteDC(dc);
            DeleteDC(hdc);

            icon_ = bitmap;
        }
    }

    MenuIcon::~MenuIcon() {
        if (icon_)
            DeleteObject(icon_);
    }

    MenuIcon::operator HBITMAP__*() const {
        return icon_;
    }

    bool MenuIcon::isValid() const {
        return icon_;
    }
}
