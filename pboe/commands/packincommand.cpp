#include "Packincommand.h"

namespace pboman3 {
    PackInCommand::PackInCommand(shared_ptr<Executable> executable, shared_ptr<vector<path>> selectedFiles)
        : executable_(move(executable)),
          selectedFiles_(move(selectedFiles)) {
    }

    HRESULT PackInCommand::GetTitle(IShellItemArray* psiItemArray, LPWSTR* ppszName) {
        const path folder = selectedFiles_->at(0).parent_path().filename();
        const wstring text = L"Pack in \"" + folder.wstring() + L"\"";
        SHStrDup(text.data(), ppszName);
        return S_OK;
    }

    HRESULT PackInCommand::Invoke(IShellItemArray* psiItemArray, IBindCtx* pbc) {
        const path cwd = selectedFiles_->at(0).parent_path();
        const HRESULT hr = executable_->packFolders(cwd, *selectedFiles_, cwd);
        return hr;
    }
}
