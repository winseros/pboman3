#pragma once

#include <ShlObj.h>
#include <filesystem>
#include "abstractcommand.h"
#include "../executable.h"

namespace pboman3 {
    class PackInCommand final : public AbstractCommand<> {
    public:
        PackInCommand(shared_ptr<Executable> executable, shared_ptr<vector<path>> selectedFiles);

        HRESULT GetTitle(IShellItemArray* psiItemArray, LPWSTR* ppszName) override;

        HRESULT Invoke(IShellItemArray* psiItemArray, IBindCtx* pbc) override;

    private:
        shared_ptr<Executable> executable_;
        shared_ptr<vector<path>> selectedFiles_;
    };
}
