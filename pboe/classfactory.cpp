#include "classfactory.h"
#include <new>
#include <Shlwapi.h>
#include "contextmenu.h"
#include "dllmain.h"

namespace pboman3 {
    ClassFactory::ClassFactory()
        : refCount_(1) {
        DllAddRef();
    }

    ClassFactory::~ClassFactory() {
        DllRelease();
    }

    HRESULT ClassFactory::QueryInterface(const IID& riid, void** ppvObject) {
        static const QITAB qit[] =
        {
            QITABENT(ClassFactory, IClassFactory),
            {0, 0}
        };
        return QISearch(this, qit, riid, ppvObject);
    }

    ULONG ClassFactory::AddRef() {
        return InterlockedIncrement(&refCount_);
    }

    ULONG ClassFactory::Release() {
        const ULONG count = InterlockedDecrement(&refCount_);
        if (count == 0)
            delete this;
        return count;
    }

    HRESULT ClassFactory::CreateInstance(IUnknown* pUnkOuter, const IID& riid, void** ppvObject) {
        *ppvObject = NULL;

        if (pUnkOuter)
            return CLASS_E_NOAGGREGATION;

        HRESULT hr = E_OUTOFMEMORY;
        auto* obj = new(std::nothrow) ContextMenu();
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
