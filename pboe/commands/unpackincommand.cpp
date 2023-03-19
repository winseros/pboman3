#include "unpackincommand.h"
#include <Shlwapi.h>

namespace pboman3 {
    UnpackInCommand::UnpackInCommand(shared_ptr<Executable> executable, shared_ptr<vector<path>> selectedFiles)
        : executable_(move(executable)),
          selectedFiles_(move(selectedFiles)) {
    }

    HRESULT UnpackInCommand::GetTitle(IShellItemArray* psiItemArray, LPWSTR* ppszName) {
        const path folder = selectedFiles_->at(0).parent_path().filename();
        const wstring text = L"Unpack in \"" + folder.wstring() + L"\"";
        SHStrDup(text.data(), ppszName);
        return S_OK;
    }

    HRESULT UnpackInCommand::Invoke(IShellItemArray* psiItemArray, IBindCtx* pbc) {
        const path cwd = selectedFiles_->at(0).parent_path();
        const HRESULT hr = executable_->unpackFiles(cwd, *selectedFiles_, cwd);
        return hr;
    }
}
