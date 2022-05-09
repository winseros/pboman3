#pragma once

#include <ShlObj.h>
#include <filesystem>
#include "abstractcommand.h"
#include "../executable.h"

namespace pboman3 {
    using namespace std;
    using namespace filesystem;

    class __declspec(uuid("dd2a27fa-7c7f-4b50-9b54-836af42fb64d")) ExplorerCommand final : public AbstractCommand<> {
    public:
        ExplorerCommand();

        //IExplorerCommand

        HRESULT GetTitle(IShellItemArray* psiItemArray, LPWSTR* ppszName) override;

        HRESULT GetIcon(IShellItemArray* psiItemArray, LPWSTR* ppszIcon) override;

        HRESULT GetState(IShellItemArray* psiItemArray, BOOL fOkToBeSlow, EXPCMDSTATE* pCmdState) override;

        HRESULT GetFlags(EXPCMDFLAGS* pFlags) override;

        HRESULT EnumSubCommands(IEnumExplorerCommand** ppEnum) override;

    private:
        shared_ptr<Executable> executable_;
        shared_ptr<vector<path>> selectedItems_;

        enum class SelectionMode {
            None,
            Mixed,
            Files,
            Folders
        };

        SelectionMode selectionMode_;

        SelectionMode getSelectionMode() const;
    };
}
