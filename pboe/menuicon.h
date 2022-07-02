#pragma once

#include <ShlObj.h>
#include <string>

namespace pboman3 {
    using namespace std;

    class MenuIcon {
    public:
        explicit MenuIcon(const wstring& exePath);

        MenuIcon(const MenuIcon&) = delete;

        MenuIcon(MenuIcon&&) = delete;

        ~MenuIcon();

        operator HBITMAP() const;

        bool isValid() const;

    private:
        HBITMAP icon_;
    };
}
