#include <QTimer>
#include <CLI/CLI.hpp>
#include "commandline.h"
#include "model/pbomodel.h"
#include "exception.h"
#include "model/task/packtask.h"
#include "model/task/unpacktask.h"
#include "io/settings/getsettingsfacility.h"
#include "util/log.h"

#define LOG(...) LOGGER("Main", __VA_ARGS__)

using namespace std;

namespace pboman3 {
    int RunConsolePackOperation(const QStringList& folders, const QString& outputDir) {
        util::UseLoggingMessagePattern();

        const auto settings = io::GetSettingsFacility()->readSettings();
        for (const QString& folder : folders) {
            //don't parallelize to avoid mess in the console
            model::task::PackTask task(folder, outputDir, settings.packConflictResolutionMode);
            task.execute([] { return false; });
        }
        return 0;
    }

    int RunConsoleUnpackOperation(const QStringList& folders, const QString& outputDir) {
        util::UseLoggingMessagePattern();
        const auto settings = io::GetSettingsFacility()->readSettings();
        for (const QString& folder : folders) {
            //don't parallelize to avoid mess in the console
            model::task::UnpackTask task(folder, outputDir, settings.unpackConflictResolutionMode);
            task.execute([] { return false; });
        }
        return 0;
    }

    template <CharOrWChar TChr>
    int RunWithCliOptions(int argc, TChr* argv[]) {
        using namespace CLI;
        using namespace pboman3;

        int exitCode;
        App cli;
        const CommandLine cmd(&cli);
        const shared_ptr<CommandLine::Result<TChr>> commandLine = cmd.build<TChr>();
        CLI11_PARSE(cli, argc, argv)

        if (commandLine->pack.hasBeenSet()) {
            QString outputDir;
            if (commandLine->pack.hasOutputPath())
                outputDir = CommandLine::toQt(commandLine->pack.outputPath);
            else
                outputDir = QDir::currentPath();

            const QStringList folders = CommandLine::toQt(commandLine->pack.folders);
            exitCode = RunConsolePackOperation(folders, outputDir);
        } else if (commandLine->unpack.hasBeenSet()) {
            QString outputDir;
            if (commandLine->unpack.hasOutputPath())
                outputDir = CommandLine::toQt(commandLine->unpack.outputPath);
            else
                outputDir = QDir::currentPath();

            const QStringList files = CommandLine::toQt(commandLine->unpack.files);
            exitCode = RunConsoleUnpackOperation(files, outputDir);
        } else {
            //should not normally get here; if did - CLI11 was misconfigured somewhere
            cout << cli.help();
            exitCode = 1;
        }
        return exitCode;
    }

    template <CharOrWChar TChr>
    int RunMain(int argc, TChr* argv[]) {
        const int exitCode = RunWithCliOptions(argc, argv);
        return exitCode;
    }
}

void HandleEptr(const std::exception_ptr& ptr) try {
    if (ptr)
        std::rethrow_exception(ptr);
} catch (const std::exception& ex) {
    LOG(critical, "Uncaught exception has been thrown:", ex.what())
}

#ifdef WIN32
int wmain(int argc, wchar_t* argv[]) {
#else
int main(int argc, char* argv[]) {
#endif
    try {
        const int res = pboman3::RunMain(argc, argv);
        return res;
    } catch (const pboman3::AppException& ex) {
        LOG(critical, "Unexpected exception has been thrown:", ex)
        throw;
    } catch (...) {
        LOG(critical, "Unexpected exception has been thrown")
        const auto ex = std::current_exception();
        HandleEptr(ex);
        return 1;
    }
}
