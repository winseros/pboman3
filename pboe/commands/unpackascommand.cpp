#include "unpackascommand.h"

namespace pboman3 {
    UnpackAsCommand::UnpackAsCommand(shared_ptr<Executable> executable, path selectedFile)
        : executable_(move(executable)),
          selectedFile_(move(selectedFile)) {
    }

    HRESULT UnpackAsCommand::GetTitle(IShellItemArray* psiItemArray, LPWSTR* ppszName) {
        const wstring text = L"Unpack as \"" + selectedFile_.filename().replace_extension().wstring() + L"\"";
        SHStrDup(text.data(), ppszName);
        return S_OK;
    }

    HRESULT UnpackAsCommand::Invoke(IShellItemArray* psiItemArray, IBindCtx* pbc) {
        const HRESULT hr = executable_->unpackFiles(selectedFile_.parent_path(), vector<path>{selectedFile_},
                                        selectedFile_.parent_path());
        return hr;
    }
}
