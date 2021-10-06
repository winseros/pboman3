#include "executable.h"
#include <numeric>
#include <filesystem>
#include "registry.h"

namespace pboman3 {
    using namespace std::filesystem;

    std::shared_ptr<Executable> Executable::fromRegistry() {
        std::string path = Registry::getExecutablePath();
        return make_shared<Executable>(path);
    }

    Executable::Executable(std::string executablePath)
        : executablePath_(std::move(executablePath)) {
    }

    bool Executable::unpackFiles(const LPCSTR cwd, const std::vector<path>& files, const std::string& path) const {
        //+some characters for command line parameter names
        const auto reserve = estimateArgvLength(files) + path.length() + 20;

        std::string argv;
        argv.reserve(reserve);

        argv.append("unpack");
        appendItemsToArgv(argv, files);
        argv.append(" -o \"").append(path).append("\"");

        const bool success = shellExecute(cwd, argv);
        return success;
    }

    bool Executable::unpackFiles(const LPCSTR cwd, const std::vector<path>& files) const {
        //+some characters for command line parameter names
        const auto reserve = estimateArgvLength(files) + 20;

        std::string argv;
        argv.reserve(reserve);

        argv.append("unpack");
        appendItemsToArgv(argv, files);
        argv.append(" -p");

        const bool success = shellExecute(cwd, argv);
        return success;
    }

    HINSTANCE Executable::packFolders(const std::vector<std::string>& folders, const std::string& path) {
        return 0;
    }

    HINSTANCE Executable::packFolders(const std::vector<std::string>& folders) {
        return 0;
    }

    bool Executable::isValid() const {
        return !executablePath_.empty()
            && is_regular_file(executablePath_);
    }

    std::size_t Executable::estimateArgvLength(const std::vector<path>& items) {
        const auto reserve = std::accumulate(items.begin(), items.end(), static_cast<std::size_t>(0),
                                             [](ULONG64 sum, const path& p) {
                                                 //each item will need 3 additional symbols: 2 quotes and separating whitespace
                                                 return sum + p.string().size() + static_cast<std::size_t>(3);
                                             });
        return reserve;
    }

    bool Executable::shellExecute(LPCSTR cwd, const std::string& argv) const {
        HINSTANCE hinst = ShellExecute(nullptr, "open", executablePath_.c_str(), argv.c_str(), cwd, SW_SHOWNORMAL);
        return reinterpret_cast<INT_PTR>(hinst) > 32;
    }

    void Executable::appendItemsToArgv(std::string& argv, const std::vector<path>& items) {
        for (const path& item : items)
            argv.append(" \"").append(item.string()).append("\"");
    }
}
