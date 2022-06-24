#include "executable.h"
#include <numeric>
#include <filesystem>
#include "registry.h"

namespace pboman3 {
    using namespace std;
    using namespace std::filesystem;

    constexpr size_t additionalCharsReserve = 20;

    std::shared_ptr<Executable> Executable::fromRegistry() {
        wstring path = Registry::getExecutablePath();
        return make_shared<Executable>(path);
    }

    Executable::Executable(wstring executablePath)
        : executablePath_(std::move(executablePath)) {
    }

    HRESULT Executable::unpackFiles(const path& cwd, const vector<path>& files, const path& outputDir) const {
        wstring argv;
        reserveArgvSize(argv, files, outputDir.wstring().length() + additionalCharsReserve);

        appendUnpackCommand(argv);
        appendPaths(argv, files);
        appendOutputDir(argv, outputDir);

        const HRESULT hr = shellExecute(cwd, argv);
        return hr;
    }

    HRESULT Executable::unpackFiles(const path& cwd, const vector<path>& files) const {
        wstring argv;
        reserveArgvSize(argv, files, additionalCharsReserve);

        appendUnpackCommand(argv);
        appendPaths(argv, files);
        appendPrompt(argv);

        const HRESULT hr = shellExecute(cwd, argv);
        return hr;
    }

    HRESULT Executable::packFolders(const path& cwd, const vector<path>& folders, const path& outputDir) const {
        wstring argv;
        reserveArgvSize(argv, folders, outputDir.wstring().length() + additionalCharsReserve);

        appendPackCommand(argv);
        appendPaths(argv, folders);
        appendOutputDir(argv, outputDir);

        const HRESULT hr = shellExecute(cwd, argv);
        return hr;
    }

    HRESULT Executable::packFolders(const path& cwd, const vector<path>& folders) const {
        wstring argv;
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

    void Executable::reserveArgvSize(wstring& argv, vector<path> items, size_t additionalSymbols) {
        const auto reserve = std::accumulate(items.begin(), items.end(), static_cast<std::size_t>(0),
                                             [](ULONG64 sum, const path& p) {
                                                 //each item will need 3 additional symbols: 2 quotes and separating whitespace
                                                 return sum + p.string().size() + static_cast<std::size_t>(3);
                                             });
        argv.reserve(reserve + additionalSymbols);
    }

    HRESULT Executable::shellExecute(const path& cwd, const wstring& argv) const {
        const HINSTANCE hinst = ShellExecute(nullptr, L"open", executablePath_.c_str(), argv.c_str(), cwd.c_str(),
                                             SW_SHOWNORMAL);
        if (reinterpret_cast<INT_PTR>(hinst) > 32) {
            return S_OK;
        }

        const HRESULT err = HRESULT_FROM_WIN32(GetLastError());
        return err;
    }

    void Executable::appendPaths(wstring& argv, const vector<path>& paths) {
        for (const path& item : paths)
            argv.append(L" \"").append(item.wstring()).append(L"\"");
    }

    void Executable::appendPackCommand(wstring& argv) {
        argv.append(L"pack");
    }

    void Executable::appendUnpackCommand(wstring& argv) {
        argv.append(L"unpack");
    }

    void Executable::appendOutputDir(wstring& argv, const path& outputDir) {
        const bool needEscaping = outputDir != outputDir.root_path();
        argv.append(L" -o ");
        if (needEscaping)
            argv.append(L"\"");
        argv.append(outputDir);
        if (needEscaping)
            argv.append(L"\"");
    }

    void Executable::appendPrompt(wstring& argv) {
        argv.append(L" -p");
    }
}
