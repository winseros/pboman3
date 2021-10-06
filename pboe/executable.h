#pragma once

#include <string>
#include <ShObjIdl.h>
#include <filesystem>
#include <memory>
#include <vector>

namespace pboman3 {
    using namespace std::filesystem;

    class Executable {
    public :
        static std::shared_ptr<Executable> fromRegistry();

        Executable(std::string executablePath);

        bool unpackFiles(const LPCSTR cwd, const std::vector<path>& files, const std::string& path) const;

        bool unpackFiles(const LPCSTR cwd, const std::vector<path>& files) const;

        HINSTANCE packFolders(const std::vector<std::string>& folders, const std::string& path);

        HINSTANCE packFolders(const std::vector<std::string>& folders);

        bool isValid() const;

    private:
        const std::string executablePath_;

        static std::size_t estimateArgvLength(const std::vector<path>& items);

        bool shellExecute(LPCSTR cwd, const std::string& argv) const;

        static void appendItemsToArgv(std::string& argv, const std::vector<path>& items);
    };
}
