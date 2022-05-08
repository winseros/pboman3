#include "Packascommand.h"

namespace pboman3 {
    PackAsCommand::PackAsCommand(shared_ptr<Executable> executable, path selectedFolder)
        : executable_(move(executable)),
          selectedFolder_(move(selectedFolder)) {
    }

    HRESULT PackAsCommand::GetTitle(IShellItemArray* psiItemArray, LPWSTR* ppszName) {
        const wstring text = L"Pack as \"" + selectedFolder_.filename().replace_extension().wstring() + L"\"";
        SHStrDup(text.data(), ppszName);
        return S_OK;
    }

    HRESULT PackAsCommand::Invoke(IShellItemArray* psiItemArray, IBindCtx* pbc) {
        const HRESULT hr = executable_->packFolders(selectedFolder_.parent_path(), vector<path>{selectedFolder_},
                                        selectedFolder_.parent_path());
        return hr;
    }
}
