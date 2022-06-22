#include <QScopedPointer>
#include <QTimer>
#include <CLI/CLI.hpp>
#include "commandline.h"
#include "model/pbomodel.h"
#include "exception.h"
#include "model/task/packtask.h"
#include "model/task/unpacktask.h"
#include "util/log.h"

#define LOG(...) LOGGER("Main", __VA_ARGS__)

using namespace std;

namespace pboman3 {
    int RunConsolePackOperation(const QStringList& folders, const QString& outputDir) {
        util::UseLoggingMessagePattern();
        for (const QString& folder : folders) {
            //don't parallelize to avoid mess in the console
            model::task::PackTask task(folder, outputDir);
            task.execute([] { return false; });
        }
        return 0;
    }

    int RunConsoleUnpackOperation(const QStringList& folders, const QString& outputDir) {
        util::UseLoggingMessagePattern();
        for (const QString& folder : folders) {
            //don't parallelize to avoid mess in the console
            model::task::UnpackTask task(folder, outputDir);
            task.execute([] { return false; });
        }
        return 0;
    }

    int RunWithCliOptions(int argc, char* argv[]) {
        using namespace CLI;
        using namespace pboman3;

        int exitCode;
        App cli;
            const CommandLine cmd(&cli);
            const shared_ptr<CommandLine::Result> commandLine = cmd.build();
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

    int RunMain(int argc, char* argv[]) {
        const int exitCode = RunWithCliOptions(argc, argv);
        return exitCode;
    }
}


int main(int argc, char* argv[]) {
    try {
        const int res = pboman3::RunMain(argc, argv);
        return res;
    } catch (const pboman3::AppException& ex) {
        LOG(critical, "Unexpected exception has been thrown:", ex);
        throw ex;
    }
}
