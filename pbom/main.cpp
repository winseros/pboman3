#include <QApplication>
#include <QScopedPointer>
#include <QTimer>
#include <Windows.h>
#include <CLI/CLI.hpp>
#include "commandline.h"
#include "model/pbomodel.h"
#include "ui/errordialog.h"
#include "ui/mainwindow.h"
#include "ui/packwindow.h"
#include "ui/unpackwindow.h"
#include "util/exception.h"
#include "util/log.h"

#define LOG(...) LOGGER("Main", __VA_ARGS__)

using namespace std;

namespace pboman3 {
    class PboApplication : public QApplication {
    public:
        PboApplication(int& argc, char** argv)
            : QApplication(argc, argv) {
        }

        bool notify(QObject* o, QEvent* e) override {
            try {
                return QApplication::notify(o, e);
            } catch (const AppException& ex) {
                LOG(warning, "The developers forgot to handle the exception in place:", ex)
                return true;
            }
        }
    };

    void ActivateCom(const QApplication& a) {
        auto* timer = new QTimer(new QTimer());
        timer->moveToThread(a.thread());
        timer->setSingleShot(true);
        QObject::connect(timer, &QTimer::timeout, [timer]() {
            LOG(info, "Initializing COM")

            const HRESULT hr = CoInitialize(nullptr);

            LOG(info, "COM init status:", hr)

            assert(SUCCEEDED(hr));
            timer->deleteLater();
        });
        timer->start();
    }

    int RunMainWindow(const QApplication& app, const QString& pboFile) {
        using namespace pboman3;

        ACTIVATE_ASYNC_LOG_SINK

        LOG(info, "Starting the app")

        ActivateCom(app);

        LOG(info, "Display the main window")
        const auto model = QScopedPointer(new model::PboModel());
        ui::MainWindow w(nullptr, model.get());
        w.show();

        if (!pboFile.isEmpty()) {
            LOG(info, "Loading the file:", pboFile)
            w.loadFile(pboFile);
        }

        const int exitCode = QApplication::exec();

        LOG(info, "The app exiting with the code:", exitCode)

        return exitCode;
    }

    int RunPackWindow(const QApplication& app, const QStringList& folders, const QString& outputDir) {
        using namespace pboman3;

        ACTIVATE_ASYNC_LOG_SINK

        LOG(info, "Starting the app")

        ActivateCom(app);

        int exitCode;
        ui::PackWindow w(nullptr);
        if (outputDir.isEmpty()) {
            exitCode = w.tryPackFoldersWithPrompt(folders) ? QApplication::exec() : 0;
        } else {
            w.packFoldersToOutputDir(folders, outputDir);
            exitCode = QApplication::exec();
        }

        LOG(info, "The app exiting with the code:", exitCode)

        return exitCode;
    }

    int RunUnpackWindow(const QApplication& app, const QStringList& files, const QString& outputDir) {
        using namespace pboman3;

        ACTIVATE_ASYNC_LOG_SINK

        LOG(info, "Starting the app")

        ActivateCom(app);

        int exitCode;
        ui::UnpackWindow w(nullptr);
        if (outputDir.isEmpty()) {
            exitCode = w.tryUnpackFilesWithPrompt(files) ? QApplication::exec() : 0;
        } else {
            w.unpackFilesToOutputDir(files, outputDir);
            exitCode = QApplication::exec();
        }

        LOG(info, "The app exiting with the code:", exitCode)

        return exitCode;
    }

    int RunWithCliOptions(int argc, char* argv[]) {
        using namespace CLI;
        using namespace pboman3;

        int exitCode;
        if (argc == 1) {
            const PboApplication app(argc, argv);
            exitCode = RunMainWindow(app, "");
        } else {
            App cli;
            const CommandLine cmd(&cli);
            const shared_ptr<CommandLine::Result> commandLine = cmd.build();
            CLI11_PARSE(cli, argc, argv)

            if (commandLine->open.hasBeenSet()) {
                const PboApplication app(argc, argv);
                const QString file = CommandLine::toQt(commandLine->open.fileName);
                exitCode = RunMainWindow(app, file);
            } else if (commandLine->pack.hasBeenSet()) {
                const PboApplication app(argc, argv);

                QString outputDir;
                if (commandLine->pack.hasOutputPath())
                    outputDir = CommandLine::toQt(commandLine->pack.outputPath);
                else if (commandLine->pack.prompt())
                    outputDir = "";
                else
                    outputDir = QDir::currentPath();

                const QStringList folders = CommandLine::toQt(commandLine->pack.folders);
                exitCode = RunPackWindow(app, folders, outputDir);
            } else if (commandLine->unpack.hasBeenSet()) {
                const PboApplication app(argc, argv);

                QString outputDir;
                if (commandLine->unpack.hasOutputPath())
                    outputDir = CommandLine::toQt(commandLine->unpack.outputPath);
                else if (commandLine->unpack.prompt())
                    outputDir = "";
                else
                    outputDir = QDir::currentPath();

                const QStringList files = CommandLine::toQt(commandLine->unpack.files);
                exitCode = RunUnpackWindow(app, files, outputDir);
            } else {
                //should not normally get here; if did - CLI11 was misconfigured somewhere
                cout << cli.help();
                exitCode = 1;
            }
        }
        return exitCode;
    }

    int RunMain(int argc, char* argv[]) {
        int exitCode;
        if (argc == 2) {
            //an escape hatch for those who won't install the app via the installer
            //and use it in "Open with" operating system feature
            //in that case the OS would call the app as "pbom <file>"
            const QString file = QString(argv[1]);
            const QFileInfo fi(file);
            if (fi.isFile() && !fi.isSymLink()) {
                const PboApplication app(argc, argv);
                exitCode = RunMainWindow(app, file);
            } else {
                //but still call regular CLI if the input argument we thought
                //might be a file - was not a file
                exitCode = RunWithCliOptions(argc, argv);
            }
        } else {
            exitCode = RunWithCliOptions(argc, argv);
        }
        return exitCode;
    }
}


int main(int argc, char* argv[]) {
    try {
        const int res = pboman3::RunMain(argc, argv);
        return res;
    } catch (...) {
    }
}
