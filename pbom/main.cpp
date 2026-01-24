#include <QApplication>
#include <QStyleHints>
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
#include "settings/getapplicationsettingsmanager.h"

#ifdef WIN32
#include "win32com.h"
#include "argv8bit.h"
#include "argv16bit.h"
#endif

#define LOG(...) LOGGER("Main", __VA_ARGS__)

using namespace std;

namespace pboman3 {
    using namespace settings;

    template <CharOrWChar TChr>
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

    void applyStyle() {
        QApplication::setStyle("fusion");
    }

    void applyColorScheme(const ApplicationSettings& settings) {
        switch (settings.applicationColorScheme) {
            case ApplicationColorScheme::Enum::Light:
                QApplication::styleHints()->setColorScheme(Qt::ColorScheme::Light);
                break;
            case ApplicationColorScheme::Enum::Dark:
                QApplication::styleHints()->setColorScheme(Qt::ColorScheme::Dark);
                break;
            default:
                QApplication::styleHints()->setColorScheme(Qt::ColorScheme::Unknown);
                break;
        }
    }

    int RunMainWindow(const QApplication& app, const QString& pboFile, const util::ApplicationLogLevel logLevel) {
        using namespace pboman3;

        ACTIVATE_ASYNC_LOG_SINK(logLevel);

        LOG(info, "Starting the app")

#ifdef WIN32
        Win32Com _(&app);
#endif

        applyStyle();
        const auto settingsManager = GetApplicationSettingsManager();
        const auto settings = settingsManager->readSettings();
        applyColorScheme(settings);

        QObject::connect(settingsManager, &ApplicationSettingsManager::settingsChanged, &applyColorScheme);

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

    int RunPackWindow(const QApplication& app, const QStringList& folders, const QString& outputDir,
                      const util::ApplicationLogLevel logLevel) {
        using namespace pboman3;
        using namespace pboman3::model::task;

        ACTIVATE_ASYNC_LOG_SINK(logLevel);

        LOG(info, "Starting the app")

#ifdef WIN32
        Win32Com _(&app);
#endif

        QString outDir = outputDir;
        if (outDir.isEmpty() && !ui::PackWindow::tryRequestTargetFolder(outDir)) {
            LOG(info, "The user refused to select the pack folder - exit now")
            return 0;
        }

        applyStyle();
        const auto settings = GetApplicationSettingsManager()->readSettings();
        applyColorScheme(settings);

        const QScopedPointer model(new PackWindowModel(folders, outDir, settings.packConflictResolutionMode));
        ui::PackWindow w(nullptr, model.get());
        w.showAndRunTasks();
        const auto exitCode = QApplication::exec();

        LOG(info, "The app exiting with the code:", exitCode)

        return exitCode;
    }

    int RunUnpackWindow(const QApplication& app, const QStringList& files, const QString& outputDir,
                        const util::ApplicationLogLevel logLevel) {
        using namespace pboman3;
        using namespace pboman3::model::task;

        ACTIVATE_ASYNC_LOG_SINK(logLevel);

        LOG(info, "Starting the app")

#ifdef WIN32
        Win32Com _(&app);
#endif

        QString outDir = outputDir;
        if (outDir.isEmpty() && !ui::UnpackWindow::tryRequestTargetFolder(outDir)) {
            LOG(info, "The user refused to select the unpack folder - exit now")
            return 0;
        }

        applyStyle();
        const auto settings = GetApplicationSettingsManager()->readSettings();
        applyColorScheme(settings);

        const QScopedPointer model(new UnpackWindowModel(files, outDir, settings.unpackConflictResolutionMode));
        ui::UnpackWindow w(nullptr, model.get());
        w.showAndRunTasks();
        const auto exitCode = QApplication::exec();

        LOG(info, "The app exiting with the code:", exitCode)

        return exitCode;
    }

    int RunConsolePackOperation(const QStringList& folders, const QString& outputDir,
                                const util::ApplicationLogLevel logLevel) {
        util::SetLoggerParameters(logLevel);

        const auto settings = GetApplicationSettingsManager()->readSettings();
        for (const QString& folder : folders) {
            //don't parallelize to avoid mess in the console
            model::task::PackTask task(folder, outputDir, settings.packConflictResolutionMode);
            task.execute([] { return false; });
        }
        return 0;
    }

    int RunConsoleUnpackOperation(const QStringList& folders, const QString& outputDir,
        const util::ApplicationLogLevel logLevel) {
        util::SetLoggerParameters(logLevel);

        const auto settings = GetApplicationSettingsManager()->readSettings();
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
        if (argc == 1) {
            const PboApplication<TChr> app(argc, argv);
            exitCode = RunMainWindow(app, "", util::ApplicationLogLevel::WARN);
        } else {
            App cli;
            const CommandLine cmd(&cli);
            const shared_ptr<CommandLine::Result<TChr>> commandLine = cmd.build<TChr>();
            CLI11_PARSE(cli, argc, argv)

            if (commandLine->open.hasBeenSet()) {
                const PboApplication<TChr> app(argc, argv);
                const QString file = CommandLine::toQt(commandLine->open.fileName);
                exitCode = RunMainWindow(app, file, commandLine->logLevel.get());
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
                    exitCode = RunConsolePackOperation(folders, outputDir, commandLine->logLevel.get());
                } else {
                    const PboApplication<TChr> app(argc, argv);
                    exitCode = RunPackWindow(app, folders, outputDir, commandLine->logLevel.get());
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
                    exitCode = RunConsoleUnpackOperation(files, outputDir, commandLine->logLevel.get());
                } else {
                    const PboApplication<TChr> app(argc, argv);
                    exitCode = RunUnpackWindow(app, files, outputDir, commandLine->logLevel.get());
                }
            } else {
                //should not normally get here; if did - CLI11 was misconfigured somewhere
                cout << cli.help();
                exitCode = 1;
            }
        }
        return exitCode;
    }

    template <CharOrWChar TChr>
    QString ReadFileName(TChr* argv[]) {
        return QString(argv[1]);
    }

#ifdef WIN32
    template <>
    QString ReadFileName(wchar_t* argv[]) {
        return QString::fromWCharArray(argv[1]);
    }
#endif

    template <CharOrWChar TChr>
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
                exitCode = RunMainWindow(app, file, util::ApplicationLogLevel::WARN);
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

template <pboman3::CharOrWChar TChr>
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

#ifdef NDEBUG
class ConsoleAttach final {
public:
    ConsoleAttach() {
        //By-default console won't work in release so user won't see command line help messages
        //this adds console to the release build application
        attached_ = AttachConsole(ATTACH_PARENT_PROCESS);
        if (attached_) {
            FILE* pFile;
            freopen_s(&pFile, "CONOUT$", "w", stdout);
            freopen_s(&pFile, "CONOUT$", "w", stderr);
            freopen_s(&pFile, "CONIN$", "r", stdin);
        }
    }

    ~ConsoleAttach() {
        if (attached_) {
            FreeConsole();
        }
    }
private:
    bool attached_;
};
#endif

int main(int argc, char* argv[]) {
#ifdef NDEBUG
    ConsoleAttach _;
#endif

    int exitCode;
#ifdef WIN32
    const pboman3::Argv16Bit arg;
    exitCode = MainImpl(arg.argc, arg.argv);
#else
    exitCode = MainImpl(argc, argv);
#endif
    return exitCode;
}
