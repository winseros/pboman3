#include "model/task/pbojsonhelper.h"

#include <QTemporaryDir>
#include <QTemporaryFile>
#include <gtest/gtest.h>
#include "domain/pbodocument.h"
#include "io/bs/pbobinarysource.h"
#include "util/filenames.h"

namespace pboman3::model::task {
    using namespace domain;

    TEST(PboJsonHelperTest, Extract_Takes_Headers) {
        const QList headers{
            QSharedPointer<DocumentHeader>(new DocumentHeader("n1", "v1")),
            QSharedPointer<DocumentHeader>(new DocumentHeader("n2", "v2")),
            QSharedPointer<DocumentHeader>(new DocumentHeader("n3", "")),
        };
        const PboDocument document("file.pbo", headers, QByteArray(1, 20));

        const PboJson options = PboJsonHelper::extractFrom(document);

        ASSERT_EQ(options.headers.count(), 3);
        ASSERT_EQ(options.headers.at(0).name, "n1");
        ASSERT_EQ(options.headers.at(0).value, "v1");
        ASSERT_EQ(options.headers.at(1).name, "n2");
        ASSERT_EQ(options.headers.at(1).value, "v2");
        ASSERT_EQ(options.headers.at(2).name, "n3");
        ASSERT_EQ(options.headers.at(2).value, "");
    }

    struct PboJsonHelperExtParam {
        QString fileWithExt;
    };

    class PboJsonHelperExtensionTest : public testing::TestWithParam<PboJsonHelperExtParam> {
    };

    TEST_P(PboJsonHelperExtensionTest, Extract_Picks_Extension_Compression) {
        const PboDocument document("file.pbo");
        document.root()->createHierarchy(PboPath("f1.p3d"));
        document.root()->createHierarchy(PboPath("f2.paa"));
        document.root()->createHierarchy(PboPath("snd/f3.ogg"));
        document.root()->createHierarchy(PboPath(GetParam().fileWithExt));

        const PboJson options = PboJsonHelper::extractFrom(document);

        ASSERT_EQ(options.compress.include.count(), 1);
        ASSERT_EQ(options.compress.include.at(0), "\\." + FileNames::getFileExtension(GetParam().fileWithExt).toLower() + "$");
    }

    INSTANTIATE_TEST_SUITE_P(PboJsonHelperTest, PboJsonHelperExtensionTest, testing::Values(                                 
                                 PboJsonHelperExtParam{"file1.txt"},
                                 PboJsonHelperExtParam{"folder1/file1.txt"},
                                 PboJsonHelperExtParam{"file1.Xml"},
                                 PboJsonHelperExtParam{"file1.cSv"}
                             ));

    struct PboJsonHelperFileParam {
        QString fileName;
        bool compressed;
    };

    class PboJsonHelperFileTest : public testing::TestWithParam<PboJsonHelperFileParam> {
    };

    TEST_P(PboJsonHelperFileTest, Extract_Picks_File_Compression) {
        QTemporaryFile file;
        file.open();
        file.close();

        const PboDocument document("file.pbo");
        PboNode* node = document.root()->createHierarchy(PboPath(GetParam().fileName));
        node->binarySource = QSharedPointer<BinarySource>(
            new io::PboBinarySource(file.fileName(), io::PboDataInfo{10, 10, 0, 0, GetParam().compressed}));

        const PboJson options = PboJsonHelper::extractFrom(document);

        if (GetParam().compressed) {
            ASSERT_EQ(options.compress.include.count(), 1);
            ASSERT_EQ(options.compress.include.at(0), "^" + GetParam().fileName.toLower() + "$");
        } else {
            ASSERT_EQ(options.compress.include.count(), 0);
        }
    }

    INSTANTIATE_TEST_SUITE_P(PboJsonHelperTest, PboJsonHelperFileTest, testing::Values(
                                 PboJsonHelperFileParam{"mission.sQm", true},
                                 PboJsonHelperFileParam{"mission.sqm", false},
                                 PboJsonHelperFileParam{"descriptIon.ext", true},
                                 PboJsonHelperFileParam{"description.ext", false}
                             ));

    TEST(PboJsonHelperTest, Extract_Picks_Multiple_Compression_Rules) {
        QTemporaryFile file;
        file.open();
        file.close();

        const PboDocument document("file.pbo");
        PboNode* node = document.root()->createHierarchy(PboPath("mission.sqm"));
        node->binarySource = QSharedPointer<BinarySource>(
            new io::PboBinarySource(file.fileName(), io::PboDataInfo{10, 10, 0, 0, true}));

        document.root()->createHierarchy(PboPath("readme.txt"));

        const PboJson options = PboJsonHelper::extractFrom(document);

        ASSERT_EQ(options.compress.include.count(), 2);
        ASSERT_EQ(options.compress.include.at(0), "\\.txt$");
        ASSERT_EQ(options.compress.include.at(1), "^mission.sqm$");
    }

    TEST(PboJsonHelperTest, SaveTo_Writes_To_File) {
        const QTemporaryDir t;

        PboJson options;
        options.headers = QList{PboJsonHeader("h1", "v1"), PboJsonHeader("h2", "v2")};
        options.compress.include = QList<QString>{"i1", "i2"};
        options.compress.exclude = QList<QString>{"e1", "e2"};

        const QString filePath = QDir(t.path()).filePath("file.txt");
        PboJsonHelper::saveTo(options, QDir(t.path()).filePath("file.txt"));

        QFile config(filePath);
        ASSERT_TRUE(config.exists());

        ASSERT_TRUE(config.open(QIODeviceBase::ReadOnly));

        const QByteArray bytes = config.readAll();
        ASSERT_EQ(QString(bytes), QString("{\n    \"compress\": {\n        \"exclude\": [\n            \"e1\",\n            \"e2\"\n        ],\n        \"include\": [\n            \"i1\",\n            \"i2\"\n        ]\n    },\n    \"headers\": [\n        {\n            \"name\": \"h1\",\n            \"value\": \"v1\"\n        },\n        {\n            \"name\": \"h2\",\n            \"value\": \"v2\"\n        }\n    ]\n}\n"));
    }

    TEST(PboJsonHelperTest, GetConfigFilePath_Picks_Non_Conflict_Name) {
        const QTemporaryDir t;
        const QDir target(t.path());

        //two placeholder files
        QFile f1(target.filePath("pbo.json"));
        f1.open(QIODeviceBase::NewOnly);
        f1.close();
        QFile f2(target.filePath("pbo(1).json"));
        f2.open(QIODeviceBase::NewOnly);
        f2.close();

        const QString filePath = PboJsonHelper::getConfigFilePath(target, FileConflictResolutionMode::Enum::Copy);

        //the resulting file prevented conflicts
        ASSERT_EQ(filePath, target.filePath("pbo(2).json"));
    }
}
