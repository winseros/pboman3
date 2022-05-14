#pragma once

#include <ShlObj.h>
#include <filesystem>
#include "abstractcommand.h"
#include "../executable.h"

namespace pboman3 {
    using namespace std;
    using namespace filesystem;

    class UnpackAsCommand final : public AbstractCommand<> {
    public:
        UnpackAsCommand(shared_ptr<Executable> executable, path selectedFile);

        //AbstractCommand

        HRESULT GetTitle(IShellItemArray* psiItemArray, LPWSTR* ppszName) override;

        HRESULT Invoke(IShellItemArray* psiItemArray, IBindCtx* pbc) override;

    private:
        shared_ptr<Executable> executable_;
        path selectedFile_;
    };
}
