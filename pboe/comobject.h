#pragma once

#include <ShlObj.h>
#include <Shlwapi.h>
#include "dllmain.h"

namespace pboman3 {
    template <class TImplementation, class... TInterface>
    class ComObject : public TInterface... {
    public:
        ComObject()
            : refCount_(1) {
            DllAddRef();
        }

        HRESULT QueryInterface(const IID& riid, void** ppvObject) override {
            const QITAB qit[] = {
                QITABENT(TImplementation, TInterface)...,
                {0, 0}
            };
            return QISearch(this, qit, riid, ppvObject);
        }

        ULONG AddRef() override {
            return InterlockedIncrement(&refCount_);
        }

        ULONG Release() override {
            const ULONG result = InterlockedDecrement(&refCount_);
            if (result == 0)
                delete this;
            return result;
        }

    protected:
        virtual ~ComObject() {
            DllRelease();
        }

    private:
        ULONG refCount_;
    };
}
