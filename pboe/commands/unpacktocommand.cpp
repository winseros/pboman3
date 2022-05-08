#include "unpacktocommand.h"
#include "../helpers.h"
#include <Shlwapi.h>

namespace pboman3 {
    UnpackToCommand::UnpackToCommand(shared_ptr<Executable> executable, shared_ptr<vector<path>> selectedFiles)
        : executable_(std::move(executable)),
          selectedFiles_(std::move(selectedFiles)) {
    }

    HRESULT UnpackToCommand::GetTitle(IShellItemArray* psiItemArray, LPWSTR* ppszName) {
        SHStrDup(L"Unpack to...", ppszName);
        return S_OK;
    }

    HRESULT UnpackToCommand::Invoke(IShellItemArray* psiItemArray, IBindCtx* pbc) {
        const path cwd = selectedFiles_->at(0).parent_path();
        const HRESULT hr = executable_->unpackFiles(cwd, *selectedFiles_);
        return hr;
    }
}
