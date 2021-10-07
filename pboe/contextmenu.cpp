#include "contextmenu.h"
#include <Shlwapi.h>
#include <cassert>
#include "dllmain.h"
#include <filesystem>
#include "registry.h"
#include <wingdi.h>
#include <strsafe.h>

// ReSharper disable CppZeroConstantCanBeReplacedWithNullptr

namespace pboman3 {
    ContextMenu::ContextMenu()
        : refCount_(1),
          subMenu_(NULL),
          icon_(NULL),
          selectedPaths_(nullptr),
          executable_(nullptr) {
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
            {0, 0}
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
        selectedPaths_ = getSelectedPaths(pdtobj);
        executable_ = Executable::fromRegistry();
        return S_OK;
    }

    constexpr int idUnpackWithPrompt = 1;
    constexpr int idUnpackToCwd = 2;
    constexpr int idPackWithPrompt = 3;
    constexpr int idPackToCwd = 4;

    HRESULT ContextMenu::QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags) {
        HRESULT hr = E_FAIL;
        if (executable_->isValid()) {
            const SelectionMode sel = getSelectionMode();
            UINT lastUsedMenuIndex;

            if (sel == SelectionMode::Files) {
                if (selectedPaths_->size() == 1) {
                    TCHAR textItem1[] = "Unpack to...";
                    insertMenuItem(idCmdFirst + idUnpackWithPrompt, textItem1);

                    string textItem2 = "Unpack as \"" + selectedPaths_->at(0)
                                                                      .filename().replace_extension().string() + "/\"";
                    insertMenuItem(idCmdFirst + idUnpackToCwd, textItem2.data());
                } else {
                    TCHAR textItem1[] = "Unpack to...";
                    insertMenuItem(idCmdFirst + idUnpackWithPrompt, textItem1);

                    string textItem2 = "Unpack in \"" + selectedPaths_->at(0).parent_path().filename().string() + "/\"";
                    insertMenuItem(idCmdFirst + idUnpackToCwd, textItem2.data());
                }

                lastUsedMenuIndex = idUnpackToCwd;
                insertRootItem(hmenu, indexMenu);
                hr = MAKE_HRESULT(SEVERITY_SUCCESS, 0, static_cast<USHORT>(lastUsedMenuIndex + 1));
            } else if (sel == SelectionMode::Folders) {
                if (selectedPaths_->size() == 1) {
                    TCHAR textItem1[] = "Pack to...";
                    insertMenuItem(idCmdFirst + idPackWithPrompt, textItem1);

                    string textItem2 = "Pack as \"" + selectedPaths_->at(0).filename().string() + ".pbo\"";
                    insertMenuItem(idCmdFirst + idPackToCwd, textItem2.data());
                } else {
                    TCHAR textItem3[] = "Pack to...";
                    insertMenuItem(idCmdFirst + idPackWithPrompt, textItem3);

                    string textItem4 = "Pack in \"" + selectedPaths_->at(0).parent_path().filename().string() + "/\"";
                    insertMenuItem(idCmdFirst + idPackToCwd, textItem4.data());
                }

                lastUsedMenuIndex = idPackToCwd;
                insertRootItem(hmenu, indexMenu);
                hr = MAKE_HRESULT(SEVERITY_SUCCESS, 0, static_cast<USHORT>(lastUsedMenuIndex + 1));
            }
        }

        return hr;
    }

    HRESULT ContextMenu::InvokeCommand(CMINVOKECOMMANDINFO* pici) {
        //https://docs.microsoft.com/en-us/windows/win32/shell/how-to-implement-the-icontextmenu-interface

        HRESULT hr = E_FAIL;

        if (pici->cbSize == sizeof CMINVOKECOMMANDINFOEX
            && pici->fMask & CMIC_MASK_UNICODE) {
            const auto piciw = reinterpret_cast<CMINVOKECOMMANDINFOEX*>(pici);
            if (HIWORD(piciw->lpVerbW) == 0) {
                //means pici contains idCmd, otherwise would mean pici contains a verb
                const auto idCmd = LOWORD(pici->lpVerb);
                switch (idCmd) {
                    case idUnpackWithPrompt:
                        hr = executable_->unpackFiles(pici->lpDirectory, *selectedPaths_);
                        break;
                    case idUnpackToCwd:
                        hr = executable_->unpackFiles(pici->lpDirectory, *selectedPaths_, pici->lpDirectory);
                        break;
                    case idPackWithPrompt:
                        hr = executable_->packFolders(pici->lpDirectory, *selectedPaths_);
                        break;
                    case idPackToCwd:
                        hr = executable_->packFolders(pici->lpDirectory, *selectedPaths_, pici->lpDirectory);
                        break;
                }
            }
        }

        return hr;
    }

    HRESULT ContextMenu::GetCommandString(UINT_PTR idCmd, UINT uType, UINT* pReserved, CHAR* pszName, UINT cchMax) {
        return E_NOTIMPL;
    }

    shared_ptr<vector<path>> ContextMenu::getSelectedPaths(IDataObject* dataObject) const {
        auto res = make_shared<vector<path>>();
        FORMATETC fe = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
        STGMEDIUM stg;
        if (SUCCEEDED(dataObject->GetData(&fe, &stg))) {
            const UINT fileCount = DragQueryFileA(static_cast<HDROP>(stg.hGlobal), 0xFFFFFFFF, NULL, 0);
            res->reserve(fileCount);
            for (UINT i = 0; i < fileCount; i++) {
                CHAR fileName[MAX_PATH];
                const UINT fileNameLen = DragQueryFileA(static_cast<HDROP>(stg.hGlobal), i, fileName, sizeof fileName);
                if (fileNameLen) {
                    const path p(string(fileName, fileNameLen));
                    if (is_directory(p) || is_regular_file(p))
                        res->emplace_back(p);
                }
            }

            ReleaseStgMedium(&stg);
        }
        return res;
    }

    ContextMenu::SelectionMode ContextMenu::getSelectionMode() const {
        auto res = SelectionMode::None;

        if (selectedPaths_ && !selectedPaths_->empty()) {
            for (const path& path : *selectedPaths_) {
                if (is_regular_file(path)) {
                    if (res == SelectionMode::Folders) {
                        res = SelectionMode::Mixed;
                        break;
                    }
                    res = SelectionMode::Files;
                } else {
                    if (res == SelectionMode::Files) {
                        res = SelectionMode::Mixed;
                        break;
                    }
                    res = SelectionMode::Folders;
                }
            }
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

    void ContextMenu::insertMenuItem(UINT wId, LPTSTR text) {
        if (!subMenu_)
            subMenu_ = CreateMenu();

        const MENUITEMINFO item = makeMenuItem(wId, text);
        InsertMenuItem(subMenu_, 0, TRUE, &item);
    }

    void ContextMenu::insertRootItem(HMENU hmenu, UINT indexMenu) const {
        TCHAR menuText[] = "PBO Manager";
        const MENUITEMINFO menu = makeRootItem(menuText, icon_, subMenu_);
        InsertMenuItem(hmenu, indexMenu, TRUE, &menu);
    }

    HBITMAP ContextMenu::loadRootIcon() const {
        const string exePath = Registry::getExecutablePath();
        if (exePath.empty() || !is_regular_file(exePath))
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
