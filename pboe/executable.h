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

        Executable(string executablePath);

        HRESULT unpackFiles(LPCSTR cwd, const vector<path>& files, const string& outputDir) const;

        HRESULT unpackFiles(LPCSTR cwd, const vector<path>& files) const;

        HRESULT packFolders(LPCSTR cwd, const vector<path>& folders, const string& outputDir) const;

        HRESULT packFolders(LPCSTR cwd, const vector<path>& folders) const;

        bool isValid() const;

    private:
        const string executablePath_;

        static void reserveArgvSize(string& argv, vector<path> items, size_t additionalSymbols);

        HRESULT shellExecute(LPCSTR cwd, const string& argv) const;

        static void appendPaths(string& argv, const vector<path>& paths);

        static void appendPackCommand(string& argv);

        static void appendUnpackCommand(string& argv);

        static void appendOutputDir(string& argv, const string& outputDir);

        static void appendPrompt(string& argv);
    };
}
