#pragma once

#include <ShlObj.h>
#include <filesystem>
#include "abstractcommand.h"
#include "../executable.h"

namespace pboman3 {
    using namespace std;
    using namespace filesystem;

    class UnpackToCommand final : public AbstractCommand<> {
    public:
        UnpackToCommand(shared_ptr<Executable> executable, shared_ptr<vector<path>> selectedFiles);

        //AbstractCommand

        HRESULT GetTitle(IShellItemArray* psiItemArray, LPWSTR* ppszName) override;

        HRESULT Invoke(IShellItemArray* psiItemArray, IBindCtx* pbc) override;

    private:
        shared_ptr<Executable> executable_;
        shared_ptr<vector<path>> selectedFiles_;
    };
}
