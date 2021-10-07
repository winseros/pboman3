#include "executable.h"
#include <numeric>
#include <filesystem>
#include "registry.h"

namespace pboman3 {
    using namespace std;
    using namespace std::filesystem;

    constexpr size_t additionalCharsReserve = 20;

    std::shared_ptr<Executable> Executable::fromRegistry() {
        std::string path = Registry::getExecutablePath();
        return make_shared<Executable>(path);
    }

    Executable::Executable(std::string executablePath)
        : executablePath_(std::move(executablePath)) {
    }

    HRESULT Executable::unpackFiles(const LPCSTR cwd, const vector<path>& files, const string& outputDir) const {
        std::string argv;
        reserveArgvSize(argv, files,  outputDir.size() + additionalCharsReserve);

        appendUnpackCommand(argv);
        appendPaths(argv, files);
        appendOutputDir(argv, outputDir);

        const HRESULT hr = shellExecute(cwd, argv);
        return hr;
    }

    HRESULT Executable::unpackFiles(const LPCSTR cwd, const vector<path>& files) const {
        std::string argv;
        reserveArgvSize(argv, files, additionalCharsReserve);

        appendUnpackCommand(argv);
        appendPaths(argv, files);
        appendPrompt(argv);

        const HRESULT hr = shellExecute(cwd, argv);
        return hr;
    }

    HRESULT Executable::packFolders(const LPCSTR cwd, const vector<path>& folders, const string& outputDir) const {
        std::string argv;
        reserveArgvSize(argv, folders, outputDir.size() + additionalCharsReserve);

        appendPackCommand(argv);
        appendPaths(argv, folders);
        appendOutputDir(argv, outputDir);

        const HRESULT hr = shellExecute(cwd, argv);
        return hr;
    }

    HRESULT Executable::packFolders(const LPCSTR cwd, const vector<path>& folders) const {
        std::string argv;
        reserveArgvSize(argv, folders, additionalCharsReserve);

        appendPackCommand(argv);
        appendPaths(argv, folders);
        appendPrompt(argv);

        const HRESULT hr = shellExecute(cwd, argv);
        return hr;
    }

    bool Executable::isValid() const {
        return !executablePath_.empty()
            && is_regular_file(executablePath_);
    }

    void Executable::reserveArgvSize(string& argv, vector<path> items, size_t additionalSymbols) {
        const auto reserve = std::accumulate(items.begin(), items.end(), static_cast<std::size_t>(0),
                                             [](ULONG64 sum, const path& p) {
                                                 //each item will need 3 additional symbols: 2 quotes and separating whitespace
                                                 return sum + p.string().size() + static_cast<std::size_t>(3);
                                             });
        argv.reserve(reserve + additionalSymbols);
    }

    HRESULT Executable::shellExecute(LPCSTR cwd, const std::string& argv) const {
        HINSTANCE hinst = ShellExecute(nullptr, "open", executablePath_.c_str(), argv.c_str(), cwd, SW_SHOWNORMAL);
        if (reinterpret_cast<INT_PTR>(hinst) > 32) {
            return S_OK;
        }

        const HRESULT err = HRESULT_FROM_WIN32(GetLastError());
        return err;
    }

    void Executable::appendPaths(string& argv, const vector<path>& paths) {
        for (const path& item : paths)
            argv.append(" \"").append(item.string()).append("\"");
    }

    void Executable::appendPackCommand(std::string& argv) {
        argv.append("pack");
    }

    void Executable::appendUnpackCommand(std::string& argv) {
        argv.append("unpack");
    }

    void Executable::appendOutputDir(std::string& argv, const std::string& outputDir) {
        argv.append(" -o \"").append(outputDir).append("\"");
    }

    void Executable::appendPrompt(std::string& argv) {
        argv.append(" -p");
    }
}
