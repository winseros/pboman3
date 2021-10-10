#include "registry.h"
#include <Windows.h>
#include "dllmain.h"

namespace pboman3 {
    HRESULT Registry::registerServer(const string& pathToExe, const string& pathToDll) {
        try {
            registerServerImpl(pathToExe, pathToDll);
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

    string Registry::getExecutablePath() {
        try {
            string value = getRegistryKeyValue("Software\\Classes\\" PBOM_SHELL_PROGID, "Path");
            return value;
        } catch (const RegistryException&) {
            return "";
        }
    }

    void Registry::registerServerImpl(const string& pathToExe, const string& pathToDll) {
        setRegistryKeyValue("Software\\Classes\\.pbo",
                            PBOM_SHELL_PROGID);
        setRegistryKeyValue("Software\\Classes\\" PBOM_SHELL_PROGID,
                            PBOM_PROJECT_NAME);
        setRegistryKeyValue("Software\\Classes\\" PBOM_SHELL_PROGID,
                            pathToExe, "Path");
        setRegistryKeyValue("Software\\Classes\\" PBOM_SHELL_PROGID "\\DefaultIcon",
                            pathToExe + ",1");
        setRegistryKeyValue("Software\\Classes\\" PBOM_SHELL_PROGID "\\Shell\\Open\\Command",
                            "\"" + pathToExe + "\" open \"%1\"");
        setRegistryKeyValue("Software\\Classes\\" PBOM_SHELL_PROGID "\\ShellEx\\ContextMenuHandlers\\pboman3",
                            PBOM_SHELL_CLSID);

        setRegistryKeyValue("Software\\Classes\\CLSID\\" PBOM_SHELL_CLSID "\\InprocServer32",
                            pathToDll);
        setRegistryKeyValue("Software\\Classes\\CLSID\\" PBOM_SHELL_CLSID "\\InprocServer32",
                            "Apartment",
                            "ThreadingModel");
        setRegistryKeyValue("Software\\Classes\\Directory\\ShellEx\\ContextMenuHandlers\\pboman3",
                            PBOM_SHELL_CLSID);
    }

    void Registry::unregisterServerImpl() {
        removeRegistryKey("Software\\Classes\\Directory\\ShellEx\\ContextMenuHandlers\\pboman3");
        removeRegistryKey("Software\\Classes\\" PBOM_SHELL_PROGID);
        removeRegistryKey("Software\\Classes\\CLSID\\" PBOM_SHELL_CLSID);
    }

    void Registry::setRegistryKeyValue(const string& key, const string& value, const string& name) {
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
                            static_cast<DWORD>(value.length()));
        RegCloseKey(reg);

        if (ls != ERROR_SUCCESS)
            throw RegistryException::fromLStatus(key, ls);
    }

    string Registry::getRegistryKeyValue(const string& key, const string& name) {
        HKEY reg;
        LSTATUS ls = RegOpenKeyEx(HKEY_CURRENT_USER,
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

        return string(vData, cbData);
    }

    void Registry::removeRegistryKey(const string& key) {
        const LSTATUS ls = RegDeleteTree(HKEY_CURRENT_USER, key.data());

        if (ls != ERROR_SUCCESS)
            throw RegistryException::fromLStatus(key, ls);
    }

    Registry::RegistryException Registry::RegistryException::fromLStatus(const string& registryKey, LSTATUS status) {
        const string systemError = getSystemMessage(status);
        return RegistryException(registryKey, systemError, status);
    }


    Registry::RegistryException::
    RegistryException(string registryKey, const string& systemError, LSTATUS status)
        : exception(systemError.data()),
          registryKey_(std::move(registryKey)),
          status_(status) {
    }

    LSTATUS Registry::RegistryException::status() const {
        return status_;
    }

    const string& Registry::RegistryException::registryKey() const {
        return registryKey_;
    }

    string Registry::RegistryException::getSystemMessage(LSTATUS status) {
        TCHAR buffer[2048];
        const DWORD size = sizeof buffer / sizeof TCHAR;

        const DWORD number = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                                           NULL,
                                           status,
                                           LANG_USER_DEFAULT,
                                           buffer,
                                           size,
                                           NULL);
        return string(buffer, number);
    }
}
