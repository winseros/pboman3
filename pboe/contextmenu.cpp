#include "contextmenu.h"
#include <Shlwapi.h>
#include <cassert>
#include "dllmain.h"
#include <filesystem>
#include "registry.h"
#include <wingdi.h>

namespace pboman3 {
    // ReSharper disable CppZeroConstantCanBeReplacedWithNullptr

    ContextMenu::ContextMenu()
        : refCount_(1),
          subMenu_(NULL),
          icon_(NULL),
          selectedPath_("") {
        DllAddRef();
    }

    ContextMenu::~ContextMenu() {
        DllRelease();
        if (subMenu_)
            DestroyMenu(subMenu_);
        if (icon_)
            DeleteObject(icon_);
    }

    HRESULT ContextMenu::QueryInterface(const IID& riid, void** ppvObject) {
        const QITAB qit[] = {
            QITABENT(ContextMenu, IContextMenu),
            QITABENT(ContextMenu, IShellExtInit),
            {0}
        };
        return QISearch(this, qit, riid, ppvObject);
    }

    ULONG ContextMenu::AddRef() {
        return InterlockedIncrement(&refCount_);
    }

    ULONG ContextMenu::Release() {
        const ULONG result = InterlockedDecrement(&refCount_);
        if (result == 0)
            delete this;
        return result;
    }

    HRESULT ContextMenu::Initialize(LPCITEMIDLIST pidlFolder, IDataObject* pdtobj, HKEY hkeyProgId) {
        icon_ = loadRootIcon();
        selectedPath_ = getSelectedPath(pdtobj);
        return S_OK;
    }

    constexpr int idUnpackTo = 0;
    constexpr int idUnpackLc = 1;
    constexpr int idPackTo = 2;
    constexpr int idPackAs = 3;

    HRESULT ContextMenu::QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags) {
        subMenu_ = CreateMenu();

        UINT lastUsedMenuIndex;
        if (is_directory(selectedPath_)) {
            TCHAR textItem1[] = "Pack as...";
            MENUITEMINFO item1 = makeMenuItem(idCmdFirst + idPackTo, textItem1);
            InsertMenuItem(subMenu_, 0, TRUE, &item1);

            string textItem2 = "Pack as \"" + selectedPath_.filename().string() + ".pbo\"";
            MENUITEMINFO item2 = makeMenuItem(idCmdFirst + idPackAs, textItem2.data());
            InsertMenuItem(subMenu_, 0, TRUE, &item2);

            lastUsedMenuIndex = idPackAs;
        } else {
            TCHAR textItem1[] = "Unpack to...";
            MENUITEMINFO item1 = makeMenuItem(idCmdFirst + idUnpackTo, textItem1);
            InsertMenuItem(subMenu_, 0, TRUE, &item1);

            string textItem2 = "Unpack to \"" + selectedPath_.filename().replace_extension().string() + "/\"";
            MENUITEMINFO item2 = makeMenuItem(idCmdFirst + idUnpackLc, textItem2.data());
            InsertMenuItem(subMenu_, 0, TRUE, &item2);

            lastUsedMenuIndex = idUnpackLc;
        }

        TCHAR menuText[] = "PBO Manager";
        MENUITEMINFO menu = makeRootItem(menuText, icon_, subMenu_);
        InsertMenuItem(hmenu, indexMenu, TRUE, &menu);

        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, static_cast<USHORT>(lastUsedMenuIndex + 1));
    }

    HRESULT ContextMenu::InvokeCommand(CMINVOKECOMMANDINFO* pici) {
        return S_OK;
    }

    HRESULT ContextMenu::GetCommandString(UINT_PTR idCmd, UINT uType, UINT* pReserved, CHAR* pszName, UINT cchMax) {
        return S_OK;
    }

    path ContextMenu::getSelectedPath(IDataObject* dataObject) const {
        string res;
        FORMATETC fe = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
        STGMEDIUM stg;
        if (SUCCEEDED(dataObject->GetData(&fe, &stg))) {
            const UINT fileCount = DragQueryFileA(static_cast<HDROP>(stg.hGlobal), 0xFFFFFFFF, NULL, 0);
            if (fileCount == 1) {
                CHAR fileName[MAX_PATH];
                const UINT fileNameLen = DragQueryFileA(static_cast<HDROP>(stg.hGlobal), 0, fileName, sizeof fileName);
                if (fileNameLen)
                    res.append(fileName, fileNameLen);
            }

            ReleaseStgMedium(&stg);
        }
        return res;
    }

    MENUITEMINFO ContextMenu::makeMenuItem(UINT wId, LPTSTR text) const {
        MENUITEMINFO item;
        item.cbSize = sizeof item;
        item.fMask = MIIM_FTYPE | MIIM_STATE | MIIM_ID | MIIM_STRING;
        item.fType = MFT_STRING;
        item.fState = MFS_ENABLED;
        item.wID = wId;
        item.dwTypeData = text;
        item.cch = sizeof item.dwTypeData;
        // ReSharper disable once CppSomeObjectMembersMightNotBeInitialized
        return item;
    }


    MENUITEMINFO ContextMenu::makeRootItem(LPTSTR text, HBITMAP icon, HMENU subMenu) const {
        MENUITEMINFO menu;
        menu.cbSize = sizeof menu;
        menu.fMask = MIIM_FTYPE | MIIM_STATE | MIIM_STRING | MIIM_SUBMENU;
        menu.fType = MFT_STRING;
        menu.fState = MFS_ENABLED;
        menu.hSubMenu = subMenu;
        menu.dwTypeData = text;
        menu.cch = sizeof menu.dwTypeData;

        if (icon) {
            menu.fMask |= MIIM_BITMAP;
            menu.hbmpItem = icon;
        }
        // ReSharper disable once CppSomeObjectMembersMightNotBeInitialized
        return menu;
    }

    HBITMAP ContextMenu::loadRootIcon() const {
        const string exePath = Registry::getExecutablePath();
        if (exePath.empty() || !std::filesystem::is_regular_file(exePath))
            return NULL;

        const int cx = GetSystemMetrics(SM_CXSMICON);
        const int cy = GetSystemMetrics(SM_CYSMICON);

        HICON icon;
        if (!ExtractIconEx(exePath.data(), 0, &icon, NULL, 1))
            return NULL;

        // ReSharper disable CppLocalVariableMayBeConst
        HDC hdc = CreateDC("DISPLAY", NULL, NULL, NULL);

        HDC dc = CreateCompatibleDC(hdc);

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

        return bitmap;
    }
}
