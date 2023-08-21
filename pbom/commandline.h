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

        template <typename TChr>
        struct CommandOpen : Command {
            basic_string<TChr> fileName;

            void configure(App* cli) override {
                command = cli->add_subcommand("open", "Open the specified PBO file");
                command->add_option("file", fileName, "The PBO file to open")
                       ->required()
                       ->check(ExistingFile);
            }
        };

        template <typename TChr>
        struct PackCommandBase : Command {
            PackCommandBase()
                : optOutputPath(nullptr)
#ifdef PBOM_GUI
                  , optPrompt(nullptr)
                  , optNoUi(nullptr)
#endif
            {
            }

            basic_string<TChr> outputPath;
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

        template <typename TChr>
        struct CommandPack : PackCommandBase<TChr> {
            vector<basic_string<TChr>> folders;

            void configure(App* cli) override {
                this->command = cli->add_subcommand("pack", "Pack the specified folders(s) as PBO(s)");

                this->command->add_option("folders", folders, "The folder(s) to pack")
                    ->required()
                    ->check(ExistingDirectory);

                this->optOutputPath = this->command->add_option("-o,--output-directory", this->outputPath,
                                                                "The directory to write the resulting PBO(s)")
                                          ->check(ExistingDirectory);

#ifdef PBOM_GUI
                this->optPrompt = this->command->add_flag("-p,--prompt",
                                                          "Show a UI dialog for the output directory selection")
                                      ->excludes(this->optOutputPath);

                this->optNoUi = this->command->add_flag("-u,--no-ui", "Run the application without the GUI")
                                    ->excludes(this->optPrompt);
#endif
            }
        };

        template <typename TChr>
        struct CommandUnpack : PackCommandBase<TChr> {
            vector<basic_string<TChr>> files;

            void configure(App* cli) override {
                this->command = cli->add_subcommand("unpack", "Unpack the specified PBO(s)");

                this->command->add_option("files", files, "The PBO(s) to unpack")
                    ->required()
                    ->check(ExistingFile);

                this->optOutputPath = this->command->add_option("-o,--output-directory", this->outputPath,
                                                                "The directory to write the PBO(s) contents")
                                          ->check(ExistingDirectory);
#ifdef PBOM_GUI
                this->optPrompt = this->command->add_flag("-p,--prompt",
                                                          "Show a UI dialog for the output directory selection");

                this->optNoUi = this->command->add_flag("-u,--no-ui", "Run the application without the GUI")
                                    ->excludes(this->optPrompt);
#endif
            }
        };

        template <typename TChr>
        struct Result {
#ifdef PBOM_GUI
            CommandOpen<TChr> open;
#endif

            CommandPack<TChr> pack;

            CommandUnpack<TChr> unpack;
        };

        explicit CommandLine(App* app)
            : app_(app) {
        }

        template <typename TChr>
        shared_ptr<Result<TChr>> build() const {
            auto result = make_shared<Result<TChr>>();
#ifdef PBOM_GUI
            result->open.configure(app_);
#endif
            result->pack.configure(app_);
            result->unpack.configure(app_);

            return result;
        }

        template <typename TChr>
        static QString toQt(const basic_string<TChr>& str) {
            return QString::fromStdString(str);
        }

        template <typename TChr>
        static QStringList toQt(const vector<basic_string<TChr>>& items) {
            QStringList qtItems;
            qtItems.reserve(static_cast<qsizetype>(items.size()));
            for_each(items.begin(), items.end(), [&qtItems](const basic_string<TChr>& item) {
                qtItems.append(toQt(item));
            });
            return qtItems;
        }

    private:
        App* app_;
    };

#ifdef WIN32
    template <>
    inline QString CommandLine::toQt(const wstring& str) {
        return QString::fromStdWString(str);
    }
#endif
}
