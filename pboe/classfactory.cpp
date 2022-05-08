#include "classfactory.h"
#include <new>
#include <Shlwapi.h>
#include "contextmenu.h"
#include "dllmain.h"
#include "commands/explorercommand.h"

namespace pboman3 {
    HRESULT ClassFactory::CreateInstance(IUnknown* pUnkOuter, const IID& riid, void** ppvObject) {
        *ppvObject = NULL;

        if (pUnkOuter)
            return CLASS_E_NOAGGREGATION;

        HRESULT hr = E_OUTOFMEMORY;
        auto* obj = new(std::nothrow) ExplorerCommand();
        if (obj) {
            hr = obj->QueryInterface(riid, ppvObject);
            obj->Release();
        }

        return hr;
    }

    HRESULT ClassFactory::LockServer(BOOL fLock) {
        if (fLock)
            DllAddRef();
        else
            DllRelease();
        return S_OK;
    }

}
