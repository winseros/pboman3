#include "explorercommand.h"
#include "folderselectionenumerator.h"
#include "fileselectionenumerator.h"
#include <Shlwapi.h>
#include <wrl/implements.h>
#include <wrl/module.h>
#include <filesystem>
#include <strsafe.h>
#include "../helpers.h"

namespace pboman3 {
    ExplorerCommand::ExplorerCommand()
        : executable_(nullptr),
          selectionMode_(SelectionMode::None) {
        //not the best idea to keep data in a moule-global vars
        //but I could not find other options for the Win11 context menu
        const wstring moduleExePath = GetModuleExecutablePath();
        executable_ = make_shared<Executable>(moduleExePath);
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
        //not the best place to init these fields
        //but I'm unaware of other options for the Win11 context menu
        selectedItems_ = make_shared<vector<path>>(GetSelectedItemsPaths(psiItemArray));
        selectionMode_ = getSelectionMode();

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
                auto enumerator = Make<FileSelectionEnumerator>(executable_, selectedItems_);
                enumerator.CopyTo(ppEnum);
                break;
            }
            case SelectionMode::Folders: {
                auto enumerator = Make<FolderSelectionEnumerator>(executable_, selectedItems_);
                enumerator.CopyTo(ppEnum);
                break;
            }
            default:
                return S_FALSE;
        }
        return S_OK;
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

    CoCreatableClass(ExplorerCommand)
    CoCreatableClassWrlCreatorMapInclude(ExplorerCommand)
}
