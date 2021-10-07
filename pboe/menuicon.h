#pragma once

#include <ShlObj.h>

namespace pboman3 {
    class MenuIcon {
    public:
        explicit MenuIcon(LPCSTR exePath);

        MenuIcon(const MenuIcon&) = delete;

        MenuIcon(MenuIcon&&) = delete;

        ~MenuIcon();

        operator HBITMAP() const;

        bool isValid() const;

    private:
        HBITMAP icon_;
    };
}
