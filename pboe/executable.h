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

        bool unpackFiles(LPCSTR cwd, const vector<path>& files, const string& outputDir) const;

        bool unpackFiles(LPCSTR cwd, const vector<path>& files) const;

        bool packFolders(LPCSTR cwd, const vector<path>& folders, const string& outputDir) const;

        bool packFolders(LPCSTR cwd, const vector<path>& folders) const;

        bool isValid() const;

    private:
        const string executablePath_;

        static void reserveArgvSize(string& argv, vector<path> items, size_t additionalSymbols);

        bool shellExecute(LPCSTR cwd, const string& argv) const;

        static void appendPaths(string& argv, const vector<path>& paths);

        static void appendPackCommand(string& argv);

        static void appendUnpackCommand(string& argv);

        static void appendOutputDir(string& argv, const string& outputDir);

        static void appendPrompt(string& argv);
    };
}
