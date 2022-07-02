#pragma once

#include <string>
#include <Windows.h>

namespace pboman3 {
    using namespace std;

    class Registry {
    public:
        static HRESULT registerServer(const wstring& pathToExe, const wstring& pathToDll);

        static HRESULT unregisterServer();

        static wstring getExecutablePath();

    private:
        static void registerServerImpl(const wstring& pathToExe, const wstring& pathToDll);

        static void unregisterServerImpl();

        static void setRegistryKeyValue(const wstring& key, const wstring& value, const wstring& name = L"");

        static wstring getRegistryKeyValue(const wstring& key, const wstring& name = L"");

        static void removeRegistryKey(const wstring& key);

        class RegistryException : public exception {
        public:
            static RegistryException fromLStatus(const wstring& registryKey, LSTATUS status);

            RegistryException(wstring registryKey, const string& systemError, LSTATUS status);

            LSTATUS status() const;

            const wstring& registryKey() const;

        private:
            wstring registryKey_;
            LSTATUS status_;

            static string getSystemMessage(LSTATUS status);
        };
    };
}
