#pragma once

#include <ShlObj.h>
#include <filesystem>
#include "executable.h"

namespace pboman3 {
    using namespace std;
    using namespace filesystem;

    class ContextMenu final : IShellExtInit, IContextMenu {
    public:
        ContextMenu();

        //IUnknown
        HRESULT QueryInterface(const IID& riid, void** ppvObject) override;

        ULONG AddRef() override;

        ULONG Release() override;

        //IShellExtInit
        HRESULT Initialize(LPCITEMIDLIST pidlFolder, IDataObject* pdtobj, HKEY hkeyProgId) override;

        //IContextMenu
        HRESULT QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags) override;

        HRESULT InvokeCommand(CMINVOKECOMMANDINFO* pici) override;

        HRESULT GetCommandString(UINT_PTR idCmd, UINT uType, UINT* pReserved, CHAR* pszName, UINT cchMax) override;

    private:
        ~ContextMenu();

        ULONG refCount_;
        HMENU subMenu_;
        HBITMAP icon_;
        shared_ptr<vector<path>> selectedPaths_;
        shared_ptr<Executable> executable_;

        shared_ptr<vector<path>> getSelectedPaths(IDataObject* dataObject) const;

        enum class SelectionMode {
            None,
            Mixed,
            Files,
            Folders
        };

        SelectionMode getSelectionMode() const;

        void insertRootItem(HMENU hmenu, UINT indexMenu) const;

        MENUITEMINFO makeRootItem(LPTSTR text, HBITMAP icon, HMENU subMenu) const;

        void insertMenuItem(UINT wId, LPTSTR text);

        MENUITEMINFO makeMenuItem(UINT wId, LPTSTR text) const;

        HBITMAP loadRootIcon() const;

        
    };
}
