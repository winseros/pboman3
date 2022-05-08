#include <Windows.h>
#include <ShlObj.h>
#include <new>
#include "dllmain.h"
#include "registry.h"
#include "classfactory.h"
#include <filesystem>

namespace pboman3 {
    ULONG DllRefCount = 0;

    void DllAddRef() {
        InterlockedIncrement(&DllRefCount);
    }

    void DllRelease() {
        InterlockedDecrement(&DllRefCount);
    }
}

HINSTANCE HInstanceDll;

STDAPI DllMain(HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        HInstanceDll = hinstDll;
        DisableThreadLibraryCalls(hinstDll);
    }
    return TRUE;
}

STDAPI DllRegisterServer() {
    using namespace std::filesystem;

    HRESULT hr;

    TCHAR buf[MAX_PATH];
    if (GetModuleFileName(HInstanceDll, buf, MAX_PATH)) {
        const path dllPath(buf);
        const path exePath = dllPath.parent_path().append(PBOM_EXECUTABLE);
        
        hr = pboman3::Registry::registerServer(exePath.wstring(), dllPath.wstring());
    } else {
        hr = HRESULT_FROM_WIN32(GetLastError());
    }

    return hr;
}

STDAPI DllUnregisterServer() {
    return pboman3::Registry::unregisterServer();
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) {
    *ppv = NULL;
    HRESULT hr = E_OUTOFMEMORY;

    auto* factory = new(std::nothrow) pboman3::ClassFactory();
    if (factory) {
        hr = factory->QueryInterface(riid, ppv);
        factory->Release();
    }

    return hr;
}

STDAPI DllCanUnloadNow() {
    return pboman3::DllRefCount == 0 ? S_OK : S_FALSE;
}
