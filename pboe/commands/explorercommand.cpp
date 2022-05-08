#include "explorercommand.h"
#include "folderselectionenumerator.h"
#include "fileselectionenumerator.h"
#include <Shlwapi.h>
#include <cassert>
#include <filesystem>
#include <wingdi.h>
#include <strsafe.h>
#include "../helpers.h"

namespace pboman3 {
    ExplorerCommand::ExplorerCommand()
        : executable_(nullptr),
          selectionMode_(SelectionMode::None) {
    }

    //IExplorerCommand

    static constexpr TCHAR commandName[] = L"PBO Manager";

    HRESULT ExplorerCommand::GetTitle(IShellItemArray* psiItemArray, LPWSTR* ppszName) {
        SHStrDup(commandName, ppszName);
        return S_OK;
    }

    HRESULT ExplorerCommand::GetIcon(IShellItemArray* psiItemArray, LPWSTR* ppszIcon) {
        std::wstring exePath = executable_->executablePath();
        exePath += L",0";
        SHStrDup(exePath.data(), ppszIcon);
        return S_OK;
    }

    HRESULT ExplorerCommand::GetState(IShellItemArray* psiItemArray, BOOL fOkToBeSlow, EXPCMDSTATE* pCmdState) {
        *pCmdState = executable_->isValid() && (selectionMode_ == SelectionMode::Files
                         || selectionMode_ == SelectionMode::Folders)
                         ? ECS_ENABLED
                         : ECS_DISABLED;
        return S_OK;
    }

    HRESULT ExplorerCommand::GetFlags(EXPCMDFLAGS* pFlags) {
        *pFlags = ECF_DEFAULT;

        if (selectionMode_ == SelectionMode::Files || selectionMode_ == SelectionMode::Folders)
            *pFlags |= ECF_HASSUBCOMMANDS;

        return S_OK;
    }

    HRESULT ExplorerCommand::EnumSubCommands(IEnumExplorerCommand** ppEnum) {
        switch (selectionMode_) {
            case SelectionMode::Files: {
                *ppEnum = new FileSelectionEnumerator(executable_, selectedItems_);
                break;
            }
            case SelectionMode::Folders: {
                *ppEnum = new FolderSelectionEnumerator(executable_, selectedItems_);
                break;
            }
            default:
                return S_FALSE;
        }
        return S_OK;
    }

    //IInitializeCommand

    HRESULT ExplorerCommand::Initialize(LPCWSTR pszCommandName, IPropertyBag* ppb) {
        VARIANT data{};
        wstring path;
        const auto hr = ppb->Read(L"Path", &data, NULL);
        if (SUCCEEDED(hr)) {
            path = wstring(data.bstrVal);
        }
        executable_ = make_shared<Executable>(path);

        return S_OK;
    }


    HRESULT ExplorerCommand::SetSelection(IShellItemArray* psia) {
        if (psia && selectionMode_ == SelectionMode::None) {
            selectedItems_ = make_shared<vector<path>>(GetSelectedItemsPaths(psia));
            selectionMode_ = getSelectionMode();
        }
        return S_OK;
    }

    HRESULT ExplorerCommand::GetSelection(const IID& riid, void** ppv) {
        return E_NOTIMPL;
    }

    // private methods

    ExplorerCommand::SelectionMode ExplorerCommand::getSelectionMode() const {
        auto res = SelectionMode::None;

        if (selectedItems_ && !selectedItems_->empty()) {
            for (const path& path : *selectedItems_) {
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
}
