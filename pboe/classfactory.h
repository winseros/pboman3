#pragma once

#include <Windows.h>

namespace pboman3 {
    class ClassFactory final : public IClassFactory {
    public:
        ClassFactory();

        //IUnknown
        HRESULT QueryInterface(const IID& riid, void** ppvObject) override;

        ULONG AddRef() override;

        ULONG Release() override;

        //IClassFactory 
        HRESULT CreateInstance(IUnknown* pUnkOuter, const IID& riid, void** ppvObject) override;

        HRESULT LockServer(BOOL fLock) override;

    private:
        ULONG refCount_;

        ~ClassFactory();
    };

}
