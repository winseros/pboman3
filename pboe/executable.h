#pragma once

#include <string>
#include <ShObjIdl.h>
#include <filesystem>
#include <memory>
#include <vector>

namespace pboman3 {
    using namespace std;
    using namespace std::filesystem;

    class Executable {
    public :
        static shared_ptr<Executable> fromRegistry();

        Executable(wstring executablePath);

        HRESULT unpackFiles(const path& cwd, const vector<path>& files, const path& outputDir) const;

        HRESULT unpackFiles(const path& cwd, const vector<path>& files) const;

        HRESULT packFolders(const path& cwd, const vector<path>& folders, const path& outputDir) const;

        HRESULT packFolders(const path& cwd, const vector<path>& folders) const;

        bool isValid() const;

    private:
        const wstring executablePath_;

        static void reserveArgvSize(wstring& argv, vector<path> items, size_t additionalSymbols);

        HRESULT shellExecute(const path& cwd, const wstring& argv) const;

        static void appendPaths(wstring& argv, const vector<path>& paths);

        static void appendPackCommand(wstring& argv);

        static void appendUnpackCommand(wstring& argv);

        static void appendOutputDir(wstring& argv, const path& outputDir);

        static void appendPrompt(wstring& argv);
    };
}
