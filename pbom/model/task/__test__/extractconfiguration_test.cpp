#include "model/task/extractconfiguration.h"

#include <QTemporaryFile>
#include <gtest/gtest.h>
#include "domain/pbodocument.h"
#include "io/bs/pbobinarysource.h"

namespace pboman3::model::task {
    using namespace domain;

    TEST(ExtractConfigurationTest, Extract_Takes_Headers) {
        const QList headers{
            QSharedPointer<DocumentHeader>(new DocumentHeader("n1", "v1")),
            QSharedPointer<DocumentHeader>(new DocumentHeader("n2", "v2")),
            QSharedPointer<DocumentHeader>(new DocumentHeader("n3", "")),
        };
        const PboDocument document("file.pbo", headers, QByteArray(1, 20));

        const PackOptions options = ExtractConfiguration::extractFrom(document);

        ASSERT_EQ(options.headers.count(), 3);
        ASSERT_EQ(options.headers.at(0).name, "n1");
        ASSERT_EQ(options.headers.at(0).value, "v1");
        ASSERT_EQ(options.headers.at(1).name, "n2");
        ASSERT_EQ(options.headers.at(1).value, "v2");
        ASSERT_EQ(options.headers.at(2).name, "n3");
        ASSERT_EQ(options.headers.at(2).value, "");
    }

    struct ExtractConfigurationExtParam {
        QString fileWithExt;
    };

    class ExtractConfigurationExtensionTest : public testing::TestWithParam<ExtractConfigurationExtParam> {
    };

    TEST_P(ExtractConfigurationExtensionTest, Extract_Picks_Extension_Compression) {
        const PboDocument document("file.pbo");
        document.root()->createHierarchy(PboPath("f1.p3d"));
        document.root()->createHierarchy(PboPath("f2.paa"));
        document.root()->createHierarchy(PboPath("snd/f3.ogg"));
        document.root()->createHierarchy(PboPath(GetParam().fileWithExt));

        const PackOptions options = ExtractConfiguration::extractFrom(document);

        ASSERT_EQ(options.compress.include.count(), 1);
        ASSERT_EQ(options.compress.include.at(0), "\\." + GetFileExtension(GetParam().fileWithExt).toLower() + "$");
    }

    INSTANTIATE_TEST_SUITE_P(ExtractConfigurationTest, ExtractConfigurationExtensionTest, testing::Values(
                                 ExtractConfigurationExtParam{"file1.sqf"},
                                 ExtractConfigurationExtParam{"folder1/file1.sqf"},
                                 ExtractConfigurationExtParam{"file1.sqs"},
                                 ExtractConfigurationExtParam{"file1.txt"},
                                 ExtractConfigurationExtParam{"file1.Xml"},
                                 ExtractConfigurationExtParam{"file1.cSv"}
                             ));

    struct ExtractConfigurationFileParam {
        QString fileName;
        bool compressed;
    };

    class ExtractConfigurationFileTest : public testing::TestWithParam<ExtractConfigurationFileParam> {
    };

    TEST_P(ExtractConfigurationFileTest, Extract_Picks_File_Compression) {
        QTemporaryFile file;
        file.open();
        file.close();

        const PboDocument document("file.pbo");
        PboNode* node = document.root()->createHierarchy(PboPath(GetParam().fileName));
        node->binarySource = QSharedPointer<BinarySource>(
            new io::PboBinarySource(file.fileName(), io::PboDataInfo{10, 10, 0, 0, GetParam().compressed}));

        const PackOptions options = ExtractConfiguration::extractFrom(document);

        if (GetParam().compressed) {
            ASSERT_EQ(options.compress.include.count(), 1);
            ASSERT_EQ(options.compress.include.at(0), "^" + GetParam().fileName.toLower() + "$");
        } else {
            ASSERT_EQ(options.compress.include.count(), 0);
        }
    }

    INSTANTIATE_TEST_SUITE_P(ExtractConfigurationTest, ExtractConfigurationFileTest, testing::Values(
                                 ExtractConfigurationFileParam{"mission.sQm", true},
                                 ExtractConfigurationFileParam{"mission.sqm", false},
                                 ExtractConfigurationFileParam{"descriptIon.ext", true},
                                 ExtractConfigurationFileParam{"description.ext", false}
                             ));

    TEST(ExtractConfigurationTest, Extract_Picks_Multiple_Compression_Rules) {
        QTemporaryFile file;
        file.open();
        file.close();

        const PboDocument document("file.pbo");
        PboNode* node = document.root()->createHierarchy(PboPath("mission.sqm"));
        node->binarySource = QSharedPointer<BinarySource>(
            new io::PboBinarySource(file.fileName(), io::PboDataInfo{10, 10, 0, 0, true}));

        document.root()->createHierarchy(PboPath("script.sqf"));

        const PackOptions options = ExtractConfiguration::extractFrom(document);

        ASSERT_EQ(options.compress.include.count(), 2);
        ASSERT_EQ(options.compress.include.at(0), "\\.sqf$");
        ASSERT_EQ(options.compress.include.at(1), "^mission.sqm$");
    }
}
