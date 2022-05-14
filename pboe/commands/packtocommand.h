#pragma once

#include <ShlObj.h>
#include <filesystem>
#include "abstractcommand.h"
#include "../executable.h"

namespace pboman3 {
    class PackToCommand final: public AbstractCommand<> {
    public:
        PackToCommand(shared_ptr<Executable> executable, shared_ptr<vector<path>> selectedFolders);

        //AbstractCommand

        HRESULT GetTitle(IShellItemArray* psiItemArray, LPWSTR* ppszName) override;

        HRESULT Invoke(IShellItemArray* psiItemArray, IBindCtx* pbc) override;

    private:
        shared_ptr<Executable> executable_;
        shared_ptr<vector<path>> selectedFolders_;
    };
}
