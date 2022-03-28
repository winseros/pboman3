#pragma once

#include <CLI/CLI.hpp>

namespace pboman3 {
    using namespace CLI;
    using namespace std;

    struct CommandLine {
        struct Command {
            Command() : command(nullptr) {
            }

            virtual ~Command() = default;

            App* command;

            bool hasBeenSet() const {
                return command->parsed();
            }

            virtual void configure(App* cli) = 0;
        };

        struct CommandOpen : Command {
            string fileName;

            void configure(App* cli) override {
                command = cli->add_subcommand("open", "Open the specified PBO file");
                command->add_option("file", fileName, "The PBO file to open")
                       ->required()
                       ->check(ExistingFile);
            }
        };

        struct PackCommandBase : Command {
            PackCommandBase()
                : optOutputPath(nullptr)
#ifdef PBOM_GUI
                  , optPrompt(nullptr)
                  , optNoUi(nullptr)
#endif
            {
            }

            string outputPath;
            Option* optOutputPath;
#ifdef PBOM_GUI
            Option* optPrompt;
            Option* optNoUi;
#endif

            bool hasOutputPath() const {
                return !!*optOutputPath;
            }
#ifdef PBOM_GUI
            bool prompt() const {
                return !!*optPrompt;
            }

            bool noUi() const {
                return !!*optNoUi;
            }
#endif
        };

        struct CommandPack : PackCommandBase {
            vector<string> folders;

            void configure(App* cli) override {
                command = cli->add_subcommand("pack", "Pack the specified folders(s) as PBO(s)");

                command->add_option("folders", folders, "The folder(s) to pack")
                       ->required()
                       ->check(ExistingDirectory);

                optOutputPath = command->add_option("-o,--output-directory", outputPath,
                                                    "The directory to write the resulting PBO(s)")
                                       ->check(ExistingDirectory);

#ifdef PBOM_GUI
                optPrompt = command->add_flag("-p,--prompt",
                                              "Show a UI dialog for the output directory selection")
                                   ->excludes(optOutputPath);

                optNoUi = command->add_flag("-u,--no-ui", "Run the application without the GUI")
                                 ->excludes(optPrompt);
#endif
            }
        };

        struct CommandUnpack : PackCommandBase {
            vector<string> files;

            void configure(App* cli) override {
                command = cli->add_subcommand("unpack", "Unpack the specified PBO(s)");

                command->add_option("files", files, "The PBO(s) to unpack")
                       ->required()
                       ->check(ExistingFile);

                optOutputPath = command->add_option("-o,--output-directory", outputPath,
                                                    "The directory to write the PBO(s) contents")
                                       ->check(ExistingDirectory);
#ifdef PBOM_GUI
                optPrompt = command->add_flag("-p,--prompt",
                                              "Show a UI dialog for the output directory selection");

                optNoUi = command->add_flag("-u,--no-ui", "Run the application without the GUI")
                                 ->excludes(optPrompt);
#endif
            }
        };

        struct Result {
#ifdef PBOM_GUI
            CommandOpen open;
#endif

            CommandPack pack;

            CommandUnpack unpack;
        };

        CommandLine(App* app)
            : app_(app) {
        }

        shared_ptr<Result> build() const {
            auto result = make_shared<Result>();
#ifdef PBOM_GUI
            result->open.configure(app_);
#endif
            result->pack.configure(app_);
            result->unpack.configure(app_);

            return result;
        }

        static QString toQt(const string& str) {
            return QString::fromStdString(str);
        }

        static QStringList toQt(const vector<string>& items) {
            QStringList qtItems;
            qtItems.reserve(static_cast<qsizetype>(items.size()));
            for_each(items.begin(), items.end(), [&qtItems](const string& item) {
                qtItems.append(toQt(item));
            });
            return qtItems;
        }

    private:
        App* app_;
    };
}
