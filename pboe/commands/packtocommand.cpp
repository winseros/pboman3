#include "packtocommand.h"
#include <Shlwapi.h>

namespace pboman3 {
    PackToCommand::PackToCommand(shared_ptr<Executable> executable, shared_ptr<vector<path>> selectedFolders)
        : executable_(move(executable)),
          selectedFolders_(move(selectedFolders)) {
    }

    HRESULT PackToCommand::GetTitle(IShellItemArray* psiItemArray, LPWSTR* ppszName) {
        SHStrDup(L"Pack to...", ppszName);
        return S_OK;
    }

    HRESULT PackToCommand::Invoke(IShellItemArray* psiItemArray, IBindCtx* pbc) {
        const path cwd = selectedFolders_->at(0).parent_path();
        const HRESULT hr = executable_->packFolders(cwd, *selectedFolders_);
        return hr;
    }
}
