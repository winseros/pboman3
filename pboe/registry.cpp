#include "registry.h"
#include <ShlObj.h>
#include <Windows.h>
#include "dllmain.h"

namespace pboman3 {
    HRESULT Registry::registerServer(const wstring& pathToExe, const wstring& pathToDll) {
        try {
            registerServerImpl(pathToExe, pathToDll);
            SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
            return S_OK;
        } catch (const RegistryException& ex) {
            return HRESULT_FROM_WIN32(ex.status());
        }
    }

    HRESULT Registry::unregisterServer() {
        try {
            unregisterServerImpl();
            return S_OK;
        } catch (const RegistryException& ex) {
            return HRESULT_FROM_WIN32(ex.status());
        }
    }

    void Registry::registerServerImpl(const wstring& pathToExe, const wstring& pathToDll) {
        setRegistryKeyValue(L"Software\\Classes\\.pbo",
                            PBOM_SHELL_PROGID);
        setRegistryKeyValue(L"Software\\Classes\\" PBOM_SHELL_PROGID,
                            L"" PBOM_PROJECT_NAME);
        setRegistryKeyValue(L"Software\\Classes\\" PBOM_SHELL_PROGID "\\DefaultIcon",
                            pathToExe + L",1");
        setRegistryKeyValue(L"Software\\Classes\\" PBOM_SHELL_PROGID "\\Shell\\Open\\Command",
                            L"\"" + pathToExe + L"\" open \"%1\"");

        setRegistryKeyValue(L"Software\\Classes\\" PBOM_SHELL_PROGID "\\Shell\\pboman3.menu",
                            PBOM_SHELL_CLSID, L"ExplorerCommandHandler");
        setRegistryKeyValue(L"Software\\Classes\\" PBOM_SHELL_PROGID "\\Shell\\pboman3.menu",
                            L"PBO Manager Context menu");
        setRegistryKeyValue(L"Software\\Classes\\" PBOM_SHELL_PROGID "\\Shell\\pboman3.menu",
                            L"", L"NeverDefault");
        setRegistryKeyValue(L"Software\\Classes\\" PBOM_SHELL_PROGID "\\Shell\\pboman3.menu",
                            pathToExe, L"Path");

        setRegistryKeyValue(L"Software\\Classes\\CLSID\\" PBOM_SHELL_CLSID,
            L"" PBOM_PROJECT_NAME);
        setRegistryKeyValue(L"Software\\Classes\\CLSID\\" PBOM_SHELL_CLSID "\\InprocServer32",
                            pathToDll);
        setRegistryKeyValue(L"Software\\Classes\\CLSID\\" PBOM_SHELL_CLSID "\\InprocServer32",
                            L"Apartment",
                            L"ThreadingModel");

        setRegistryKeyValue(L"Software\\Classes\\Directory\\Shell\\pboman3.menu",
                            PBOM_SHELL_CLSID, L"ExplorerCommandHandler");
        setRegistryKeyValue(L"Software\\Classes\\Directory\\Shell\\pboman3.menu",
                            L"PBO Manager Context menu");
        setRegistryKeyValue(L"Software\\Classes\\Directory\\Shell\\pboman3.menu",
                            L"", L"NeverDefault");
        setRegistryKeyValue(L"Software\\Classes\\Directory\\Shell\\pboman3.menu",
                            pathToExe, L"Path");
    }

    void Registry::unregisterServerImpl() {
        removeRegistryKey(L"Software\\Classes\\Directory\\shell\\pboman3.menu");
        removeRegistryKey(L"Software\\Classes\\" PBOM_SHELL_PROGID);
        removeRegistryKey(L"Software\\Classes\\CLSID\\" PBOM_SHELL_CLSID);
    }

    void Registry::setRegistryKeyValue(const wstring& key, const wstring& value, const wstring& name) {
        HKEY reg;
        LSTATUS ls = RegCreateKeyEx(HKEY_CURRENT_USER,
                                    key.data(),
                                    0,
                                    NULL,
                                    REG_OPTION_NON_VOLATILE,
                                    KEY_SET_VALUE | KEY_WOW64_64KEY,
                                    NULL,
                                    &reg,
                                    NULL);

        if (ls != ERROR_SUCCESS)
            throw RegistryException::fromLStatus(key, ls);

        ls = RegSetKeyValue(reg, NULL, name.empty() ? NULL : name.data(), REG_SZ, value.data(),
                            static_cast<DWORD>(value.length() * sizeof TCHAR));
        RegCloseKey(reg);

        if (ls != ERROR_SUCCESS)
            throw RegistryException::fromLStatus(key, ls);
    }

    wstring Registry::getRegistryKeyValue(const wstring& key, const wstring& name) {
        HKEY reg;
        LSTATUS ls = RegOpenKeyEx(HKEY_CLASSES_ROOT,
                                  key.data(),
                                  NULL,
                                  KEY_QUERY_VALUE | KEY_WOW64_64KEY,
                                  &reg);

        if (ls != ERROR_SUCCESS)
            throw RegistryException::fromLStatus(key, ls);

        TCHAR vData[2048];
        DWORD cbData = sizeof vData / sizeof TCHAR;

        ls = RegGetValue(reg, NULL, name.empty() ? NULL : name.data(), RRF_RT_REG_SZ, NULL, &vData, &cbData);
        RegCloseKey(reg);

        if (ls != ERROR_SUCCESS)
            throw RegistryException::fromLStatus(key, ls);

        return wstring(vData, cbData);
    }

    void Registry::removeRegistryKey(const wstring& key) {
        const LSTATUS ls = RegDeleteTree(HKEY_CURRENT_USER, key.data());

        if (ls != ERROR_SUCCESS)
            throw RegistryException::fromLStatus(key, ls);
    }

    Registry::RegistryException Registry::RegistryException::fromLStatus(const wstring& registryKey, LSTATUS status) {
        const string systemError = getSystemMessage(status);
        return RegistryException(registryKey, systemError, status);
    }


    Registry::RegistryException::
    RegistryException(wstring registryKey, const string& systemError, LSTATUS status)
        : exception(systemError.data()),
          registryKey_(std::move(registryKey)),
          status_(status) {
    }

    LSTATUS Registry::RegistryException::status() const {
        return status_;
    }

    const wstring& Registry::RegistryException::registryKey() const {
        return registryKey_;
    }

    string Registry::RegistryException::getSystemMessage(LSTATUS status) {
        CHAR buffer[2048];
        const DWORD size = sizeof buffer / sizeof CHAR;

        const DWORD number = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,
                                            NULL,
                                            status,
                                            LANG_USER_DEFAULT,
                                            buffer,
                                            size,
                                            NULL);
        return string(buffer, number);
    }
}
