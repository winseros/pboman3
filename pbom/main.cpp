#include <QApplication>
#include <QScopedPointer>
#include <QTimer>
#include <CLI/CLI.hpp>
#include "commandline.h"
#include "model/pbomodel.h"
#include "ui/errordialog.h"
#include "ui/mainwindow.h"
#include "ui/packwindow.h"
#include "ui/unpackwindow.h"
#include "exception.h"
#include "model/task/packtask.h"
#include "model/task/unpacktask.h"
#include "util/log.h"
#include "io/settings/getsettingsfacility.h"

#ifdef WIN32
#include "win32com.h"
#include "argv8bit.h"
#include "argv16bit.h"
#endif

#define LOG(...) LOGGER("Main", __VA_ARGS__)

using namespace std;

namespace pboman3 {
    template <typename TChr>
    class PboApplication : public QApplication {
    public:
        PboApplication(int& argc, TChr* argv[])
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

#ifdef WIN32
    template <>
    class PboApplication<wchar_t> : public QApplication {
    public:
        PboApplication(int& argc, wchar_t* argv[])
            : QApplication(argc, pboman3::Argv8Bit::acquire(argc, argv)) {
        }

        ~PboApplication() override {
            pboman3::Argv8Bit::release();
        }
    };
#endif

    int RunMainWindow(const QApplication& app, const QString& pboFile) {
        using namespace pboman3;

        ACTIVATE_ASYNC_LOG_SINK

        LOG(info, "Starting the app")

#ifdef WIN32
        Win32Com _(&app);
#endif

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
        using namespace pboman3::model::task;

        ACTIVATE_ASYNC_LOG_SINK

        LOG(info, "Starting the app")

#ifdef WIN32
        Win32Com _(&app);
#endif

        QString outDir = outputDir;
        if (outDir.isEmpty() && !ui::PackWindow::tryRequestTargetFolder(outDir)) {
            LOG(info, "The user refused to select the pack folder - exit now")
            return 0;
        }

        const QSharedPointer<ApplicationSettingsFacility> settingsFacility = GetSettingsFacility();
        const auto settings = settingsFacility->readSettings();

        const QScopedPointer model(new PackWindowModel(folders, outDir, settings.packConflictResolutionMode));
        ui::PackWindow w(nullptr, model.get());
        w.showAndRunTasks();
        const auto exitCode = QApplication::exec();

        LOG(info, "The app exiting with the code:", exitCode)

        return exitCode;
    }

    int RunUnpackWindow(const QApplication& app, const QStringList& files, const QString& outputDir) {
        using namespace pboman3;
        using namespace pboman3::model::task;

        ACTIVATE_ASYNC_LOG_SINK

        LOG(info, "Starting the app")

#ifdef WIN32
        Win32Com _(&app);
#endif

        QString outDir = outputDir;
        if (outDir.isEmpty() && !ui::UnpackWindow::tryRequestTargetFolder(outDir)) {
            LOG(info, "The user refused to select the unpack folder - exit now")
            return 0;
        }

        const QSharedPointer<ApplicationSettingsFacility> settingsFacility = GetSettingsFacility();
        const auto settings = settingsFacility->readSettings();

        const QScopedPointer model(new UnpackWindowModel(files, outDir, settings.unpackConflictResolutionMode));
        ui::UnpackWindow w(nullptr, model.get());
        w.showAndRunTasks();
        const auto exitCode = QApplication::exec();

        LOG(info, "The app exiting with the code:", exitCode)

        return exitCode;
    }

    int RunConsolePackOperation(const QStringList& folders, const QString& outputDir) {
        util::UseLoggingMessagePattern();
        const QSharedPointer<io::ApplicationSettingsFacility> settingsFacility = io::GetSettingsFacility();
        const auto settings = settingsFacility->readSettings();
        for (const QString& folder : folders) {
            //don't parallelize to avoid mess in the console
            model::task::PackTask task(folder, outputDir, settings.packConflictResolutionMode);
            task.execute([] { return false; });
        }
        return 0;
    }

    int RunConsoleUnpackOperation(const QStringList& folders, const QString& outputDir) {
        util::UseLoggingMessagePattern();
        const QSharedPointer<io::ApplicationSettingsFacility> settingsFacility = io::GetSettingsFacility();
        const auto settings = settingsFacility->readSettings();
        for (const QString& folder : folders) {
            //don't parallelize to avoid mess in the console
            model::task::UnpackTask task(folder, outputDir, settings.unpackConflictResolutionMode);
            task.execute([] { return false; });
        }
        return 0;
    }

    template <typename TChr>
    int RunWithCliOptions(int argc, TChr* argv[]) {
        using namespace CLI;
        using namespace pboman3;

        int exitCode;
        if (argc == 1) {
            const PboApplication<TChr> app(argc, argv);
            exitCode = RunMainWindow(app, "");
        } else {
            App cli;
            const CommandLine cmd(&cli);
            const shared_ptr<CommandLine::Result<TChr>> commandLine = cmd.build<TChr>();
            CLI11_PARSE(cli, argc, argv)

            if (commandLine->open.hasBeenSet()) {
                const PboApplication<TChr> app(argc, argv);
                const QString file = CommandLine::toQt(commandLine->open.fileName);
                exitCode = RunMainWindow(app, file);
            } else if (commandLine->pack.hasBeenSet()) {
                QString outputDir;
                if (commandLine->pack.hasOutputPath())
                    outputDir = CommandLine::toQt(commandLine->pack.outputPath);
                else if (commandLine->pack.prompt())
                    outputDir = "";
                else
                    outputDir = QDir::currentPath();

                const QStringList folders = CommandLine::toQt(commandLine->pack.folders);
                if (commandLine->pack.noUi()) {
                    exitCode = RunConsolePackOperation(folders, outputDir);
                } else {
                    const PboApplication<TChr> app(argc, argv);
                    exitCode = RunPackWindow(app, folders, outputDir);
                }
            } else if (commandLine->unpack.hasBeenSet()) {
                QString outputDir;
                if (commandLine->unpack.hasOutputPath())
                    outputDir = CommandLine::toQt(commandLine->unpack.outputPath);
                else if (commandLine->unpack.prompt())
                    outputDir = "";
                else
                    outputDir = QDir::currentPath();

                const QStringList files = CommandLine::toQt(commandLine->unpack.files);
                if (commandLine->unpack.noUi()) {
                    exitCode = RunConsoleUnpackOperation(files, outputDir);
                } else {
                    const PboApplication<TChr> app(argc, argv);
                    exitCode = RunUnpackWindow(app, files, outputDir);
                }
            } else {
                //should not normally get here; if did - CLI11 was misconfigured somewhere
                cout << cli.help();
                exitCode = 1;
            }
        }
        return exitCode;
    }

    template <typename TChr>
    QString ReadFileName(TChr* argv[]) {
        return QString(argv[1]);
    }

#ifdef WIN32
    template <>
    QString ReadFileName(wchar_t* argv[]) {
        return QString::fromWCharArray(argv[1]);
    }
#endif


    template <typename TChr>
    int RunMain(int argc, TChr* argv[]) {
        int exitCode;
        if (argc == 2) {
            //an escape hatch for those who won't install the app via the installer
            //and use it in "Open with" operating system feature
            //in that case the OS would call the app as "pbom <file>"

            const QString file = ReadFileName(argv);

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

void HandleEptr(const std::exception_ptr& ptr) try {
    if (ptr)
        std::rethrow_exception(ptr);
} catch (const std::exception& ex) {
    LOG(critical, "Uncaught exception has been thrown:", ex.what())
}

template <typename TChr>
int MainImpl(int argc, TChr* argv[]) {
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

int main(int argc, char* argv[]) {
#ifdef WIN32
    const pboman3::Argv16Bit arg;
    MainImpl(arg.argc, arg.argv);
#else
    MainImpl(argc, argv);
#endif
}
