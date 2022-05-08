#pragma once

#include <Windows.h>
#include "comobject.h"

namespace pboman3 {
    class ClassFactory final : public ComObject<ClassFactory, IClassFactory> {
    public:
        //IClassFactory

        HRESULT CreateInstance(IUnknown* pUnkOuter, const IID& riid, void** ppvObject) override;

        HRESULT LockServer(BOOL fLock) override;
    };

}
