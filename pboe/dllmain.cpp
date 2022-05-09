#include <Windows.h>
#include <ShlObj.h>
#include <new>
#include "dllmain.h"
#include "registry.h"
#include <filesystem>

#include <wrl/module.h>
#include <wrl/implements.h>
#include <wrl/client.h>
#include <shobjidl_core.h>

HINSTANCE HInstanceDll;

namespace pboman3 {
    wstring GetModuleExecutablePath() {
        using namespace std::filesystem;

        TCHAR buf[MAX_PATH];
        if (GetModuleFileName(HInstanceDll, buf, MAX_PATH)) {
            const path dllPath(buf);
            const path exePath = dllPath.parent_path().append(PBOM_EXECUTABLE);
            return exePath.wstring();
        }
        return wstring();
    }
}

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
    using namespace Microsoft::WRL;
    HRESULT hr = Module<InProc>::GetModule().GetClassObject(rclsid, riid, ppv);
    return hr;
}

STDAPI DllGetActivationFactory(_In_ HSTRING activatableClassId, _COM_Outptr_ IActivationFactory** factory)
{
    using namespace Microsoft::WRL;
    HRESULT hr = Module<ModuleType::InProc>::GetModule().GetActivationFactory(activatableClassId, factory);
    return hr;
}

STDAPI DllCanUnloadNow() {
    using namespace Microsoft::WRL;
    HRESULT hr = Module<InProc>::GetModule().GetObjectCount() == 0 ? S_OK : S_FALSE;
    return hr;
}
