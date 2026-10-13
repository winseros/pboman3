#include <gtest/gtest.h>
#include <QFile>
#include <QTemporaryDir>
#include "commandline.h"

namespace pboman3 {
    namespace {
        void Parse(CLI::App& app, std::vector<std::string> arguments) {
            std::vector<char*> argv;
            argv.reserve(arguments.size());
            for (std::string& argument : arguments)
                argv.push_back(argument.data());
            app.parse(static_cast<int>(argv.size()), argv.data());
        }
    }

    TEST(CommandLineTest, UsePboPrefixIsFalseWhenFlagIsAbsent) {
        QTemporaryDir dir;
        const QString path = dir.filePath("input.pbo");
        QFile file(path);
        ASSERT_TRUE(file.open(QIODevice::WriteOnly));
        file.close();

        CLI::App app;
        const auto commandLine = CommandLine(&app).build<char>();
        Parse(app, {"pboc", "unpack", "--", path.toStdString()});

        EXPECT_FALSE(commandLine->unpack.usePboPrefix());
    }

    TEST(CommandLineTest, UsePboPrefixIsTrueWhenFlagIsPresent) {
        QTemporaryDir dir;
        const QString path = dir.filePath("input.pbo");
        QFile file(path);
        ASSERT_TRUE(file.open(QIODevice::WriteOnly));
        file.close();

        CLI::App app;
        const auto commandLine = CommandLine(&app).build<char>();
        Parse(app, {"pboc", "unpack", "--use-pbo-prefix", "--", path.toStdString()});

        EXPECT_TRUE(commandLine->unpack.usePboPrefix());
    }

    TEST(CommandLineTest, BesideInputIsMutuallyExclusiveWithOutputDirectory) {
        QTemporaryDir input;
        QTemporaryDir output;

        CLI::App app;
        CommandLine(&app).build<char>();

        EXPECT_THROW(Parse(app, {"pboc", "pack", "--beside-input", "--output-directory",
                                 output.path().toStdString(), "--", input.path().toStdString()}),
                     CLI::ExcludesError);
    }

    TEST(CommandLineTest, BesideInputIsReportedForPackCommand) {
        QTemporaryDir input;

        CLI::App app;
        const auto commandLine = CommandLine(&app).build<char>();
        Parse(app, {"pboc", "pack", "--beside-input", "--", input.path().toStdString()});

        EXPECT_TRUE(commandLine->pack.besideInput());
        EXPECT_FALSE(commandLine->pack.hasOutputPath());
    }
}
