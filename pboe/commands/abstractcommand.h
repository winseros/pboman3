#pragma once

#include <ShlObj.h>
#include "../comobject.h"

namespace pboman3 {
    template <class TImplementation, class... TInterface>
    class AbstractCommand : public ComObject<TImplementation, IExplorerCommand, TInterface...> {
    public:
        //IExplorerCommand

        HRESULT GetTitle(IShellItemArray* psiItemArray, LPWSTR* ppszName) override = 0;

        HRESULT GetIcon(IShellItemArray* psiItemArray, LPWSTR* ppszIcon) override {
            return E_NOTIMPL;
        }

        HRESULT GetToolTip(IShellItemArray* psiItemArray, LPWSTR* ppszInfotip) override {
            return E_NOTIMPL;
        }

        HRESULT GetCanonicalName(GUID* pguidCommandName) override {
            return E_NOTIMPL;
        }

        HRESULT GetState(IShellItemArray* psiItemArray, BOOL fOkToBeSlow, EXPCMDSTATE* pCmdState) override {
            *pCmdState = ECS_ENABLED;
            return S_OK;
        }

        HRESULT Invoke(IShellItemArray* psiItemArray, IBindCtx* pbc) override {
            return E_NOTIMPL;
        }

        HRESULT GetFlags(EXPCMDFLAGS* pFlags) override {
            *pFlags = ECF_DEFAULT;
            return S_OK;
        }

        HRESULT EnumSubCommands(IEnumExplorerCommand** ppEnum) override {
            return E_NOTIMPL;
        }
    };
}
