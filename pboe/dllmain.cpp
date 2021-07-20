#include <Windows.h>
#include <ShlObj.h>
#include <new>
#include "dllmain.h"
#include "registry.h"
#include <cstdio>
#include "classfactory.h"

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

LSTATUS RegGetValueAtKey(HKEY hKey, LPCTSTR lpSubKey, PVOID pvData, LPDWORD pcbData) {
    HKEY key;
    LSTATUS ls = RegCreateKeyEx(hKey,
                                lpSubKey,
                                0,
                                NULL,
                                REG_OPTION_NON_VOLATILE,
                                KEY_QUERY_VALUE | KEY_WOW64_64KEY,
                                NULL,
                                &key,
                                NULL);

    if (ls == ERROR_SUCCESS) {
        ls = RegGetValue(key, NULL, NULL, RRF_RT_REG_SZ, NULL, pvData, pcbData);
        RegCloseKey(key);
    }

    return ls;
}

STDAPI DllRegisterServer() {
#define EXE_PATH "C:\\Users\\winse\\Desktop\\pboman3\\out\\build\\x64-Debug\\pbom\\pbom.exe"
#define DLL_PATH "C:\\Users\\winse\\Desktop\\pboman3\\out\\build\\x64-Debug\\pboe\\pboe.dll"
    return pboman3::Registry::registerServer(EXE_PATH, DLL_PATH);
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
    return pboman3::DllRefCount == 0 ? S_FALSE : S_OK;
}
