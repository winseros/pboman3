#pragma once

#include <CLI/CLI.hpp>
#include "util/applicationloglevel.h"

namespace pboman3 {
    using namespace CLI;
    using namespace std;

    template <typename TChr>
    concept CharOrWChar = is_same_v<TChr, char> || is_same_v<TChr, wchar_t>;

    struct CommandLine {
        struct Command {
            Command() : command(nullptr) {
            }

            virtual ~Command() = default;

            App* command;

            [[nodiscard]] bool hasBeenSet() const {
                return command->parsed();
            }

            virtual void configure(App* cli) = 0;
        };

        template <CharOrWChar TChr>
        struct CommandOpen : Command {
            basic_string<TChr> fileName;

            void configure(App* cli) override {
                command = cli->add_subcommand("open", "Open the specified PBO file");
                command->add_option("file", fileName, "The PBO file to open")
                       ->required()
                       ->check(ExistingFile);
            }
        };

        template <CharOrWChar TChr>
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

            [[nodiscard]] bool hasOutputPath() const {
                return !!*optOutputPath;
            }
#ifdef PBOM_GUI
            [[nodiscard]] bool prompt() const {
                return !!*optPrompt;
            }

            [[nodiscard]] bool noUi() const {
                return !!*optNoUi;
            }
#endif
        };

        template <CharOrWChar TChr>
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

        template <CharOrWChar TChr>
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

        struct Parameter {
            Parameter(): option(nullptr) {
            }

            virtual ~Parameter() = default;

            Option* option;

            virtual void configure(App* cli) = 0;
        };

        struct ParameterLogLevel : Parameter {
            std::string value;

#define LVL_WARN "warn"
#define LVL_INFO "info"
#define LVL_DEBUG "debug"

            void configure(App* app) override {
                option = app->add_option("-l,--log-level", value, "Log output level [warn,info,debug]")
                                   ->option_text(" ")
                                   ->default_val(LVL_WARN)
                                   ->check(IsMember({LVL_WARN, LVL_INFO, LVL_DEBUG}));
            }
            [[nodiscard]] util::ApplicationLogLevel get() const {
                if (value == LVL_DEBUG) {
                    return util::ApplicationLogLevel::DEBUG;
                }
                if (value == LVL_INFO) {
                    return util::ApplicationLogLevel::INFO;
                }
                return util::ApplicationLogLevel::WARN;
            }
        };

        template <CharOrWChar TChr>
        struct Result {
#ifdef PBOM_GUI
            CommandOpen<TChr> open;
#endif

            CommandPack<TChr> pack;

            CommandUnpack<TChr> unpack;

            ParameterLogLevel logLevel;
        };

        explicit CommandLine(App* app)
            : app_(app) {
        }

        template <CharOrWChar TChr>
        shared_ptr<Result<TChr>> build() const {
            auto result = make_shared<Result<TChr>>();
#ifdef PBOM_GUI
            result->open.configure(app_);
#endif
            result->pack.configure(app_);
            result->unpack.configure(app_);
            result->logLevel.configure(app_);

            return result;
        }

        template <CharOrWChar TChr>
        static QString toQt(const basic_string<TChr>& str) {
            return QString::fromStdString(str);
        }

        template <CharOrWChar TChr>
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
