#include "model/task/packconfiguration.h"
#include <QTemporaryFile>
#include <gtest/gtest.h>
#include "domain/pbodocument.h"
#include "io/bs/fslzhbinarysource.h"
#include "io/bs/fsrawbinarysource.h"

namespace pboman3::model::task::test {
    using namespace domain;

    TEST(PackConfigurationTest, Apply_Removes_PboJson_Node) {
        QTemporaryFile json;
        json.open();
        json.write(QByteArray("{}"));
        json.close();

        PboDocument document("file.pbo");
        document.root()->createHierarchy(PboPath({"f1.txt"}));
        PboNode* pboJson = document.root()->createHierarchy(PboPath({"pbo.json"}));
        pboJson->binarySource = QSharedPointer<BinarySource>(new io::FsRawBinarySource(json.fileName()));

        const PackConfiguration packConfiguration(&document);
        packConfiguration.apply();

        ASSERT_EQ(document.root()->count(), 1); //pbo.json removed
        ASSERT_TRUE(document.root()->get(PboPath({"f1.txt"}))); //but others are in places
    }

    TEST(PackConfigurationTest, Apply_Sets_Headers_From_PboJson) {
        QTemporaryFile json;
        json.open();
        json.write(QByteArray(
            "{\"headers\":[{\"name\":\"p1\", \"value\":\"v1\"}, {\"name\":\"p2\", \"value\":\"v2\"}]}"));
        json.close();

        PboDocument document("file.pbo");
        PboNode* pboJson = document.root()->createHierarchy(PboPath({"pbo.json"}));
        pboJson->binarySource = QSharedPointer<BinarySource>(new io::FsRawBinarySource(json.fileName()));

        const PackConfiguration packConfiguration(&document);
        packConfiguration.apply();

        ASSERT_EQ(document.headers()->count(), 2);
        ASSERT_EQ(document.headers()->at(0)->name(), "p1");
        ASSERT_EQ(document.headers()->at(0)->value(), "v1");
        ASSERT_EQ(document.headers()->at(1)->name(), "p2");
        ASSERT_EQ(document.headers()->at(1)->value(), "v2");
    }

    TEST(PackConfigurationTest, Apply_Compresses_Only_Included_Files) {
        QTemporaryFile json;
        json.open();
        json.write(QByteArray("{\"compress\":{\"include\":[\".+\\.txt$\"]}}"));
        json.close();

        PboDocument document("file.pbo");
        PboNode* pboJson = document.root()->createHierarchy(PboPath({"pbo.json"}));
        pboJson->binarySource = QSharedPointer<BinarySource>(new io::FsRawBinarySource(json.fileName()));

        PboNode* node1 = document.root()->createHierarchy(PboPath({"file.txt"}));
        node1->binarySource = QSharedPointer<BinarySource>(new io::FsRawBinarySource(json.fileName()));
        PboNode* node2 = document.root()->createHierarchy(PboPath({"file2.txt"}));
        node2->binarySource = QSharedPointer<BinarySource>(new io::FsRawBinarySource(json.fileName()));
        PboNode* node3 = document.root()->createHierarchy(PboPath({"file3.jpg"}));
        node3->binarySource = QSharedPointer<BinarySource>(new io::FsRawBinarySource(json.fileName()));

        const PackConfiguration packConfiguration(&document);
        packConfiguration.apply();

        ASSERT_TRUE(dynamic_cast<io::FsLzhBinarySource*>(node1->binarySource.get()));
        ASSERT_TRUE(dynamic_cast<io::FsLzhBinarySource*>(node2->binarySource.get()));
        ASSERT_TRUE(dynamic_cast<io::FsRawBinarySource*>(node3->binarySource.get()));
    }

    TEST(PackConfigurationTest, Apply_Not_Compresses_Inlcuded_But_Excluded_Files) {
        QTemporaryFile json;
        json.open();
        json.write(QByteArray("{\"compress\":{\"include\":[\"\\.txt$\"], \"exclude\":[\"^file3\\.\"]}}"));
        json.close();

        PboDocument document("file.pbo");
        PboNode* pboJson = document.root()->createHierarchy(PboPath({"pbo.json"}));
        pboJson->binarySource = QSharedPointer<BinarySource>(new io::FsRawBinarySource(json.fileName()));

        PboNode* node1 = document.root()->createHierarchy(PboPath({"file.txt"}));
        node1->binarySource = QSharedPointer<BinarySource>(new io::FsRawBinarySource(json.fileName()));
        PboNode* node2 = document.root()->createHierarchy(PboPath({"file2.txt"}));
        node2->binarySource = QSharedPointer<BinarySource>(new io::FsRawBinarySource(json.fileName()));
        PboNode* node3 = document.root()->createHierarchy(PboPath({"file3.txt"}));
        node3->binarySource = QSharedPointer<BinarySource>(new io::FsRawBinarySource(json.fileName()));

        const PackConfiguration packConfiguration(&document);
        packConfiguration.apply();

        ASSERT_TRUE(dynamic_cast<io::FsLzhBinarySource*>(node1->binarySource.get()));
        ASSERT_TRUE(dynamic_cast<io::FsLzhBinarySource*>(node2->binarySource.get()));
        ASSERT_TRUE(dynamic_cast<io::FsRawBinarySource*>(node3->binarySource.get()));
    }

    struct PackConfigurationJsonIssuesParam {
        QString json;
        QString expectedMessage;
    };

    class PackConfigurationJsonIssuesTest : public testing::TestWithParam<PackConfigurationJsonIssuesParam> {
    };

    TEST_P(PackConfigurationJsonIssuesTest, Apply_Throws_If_PboJson_Has_Issues) {
        QTemporaryFile json;
        json.open();
        json.write(GetParam().json.toUtf8());
        json.close();

        PboDocument document("file.pbo");
        PboNode* pboJson = document.root()->createHierarchy(PboPath({"pbo.json"}));
        pboJson->binarySource = QSharedPointer<BinarySource>(new io::FsRawBinarySource(json.fileName()));

        const PackConfiguration packConfiguration(&document);
        try {
            packConfiguration.apply();
            FAIL() << "Should not have reached this line";
        } catch (const JsonStructureException& ex) {
            ASSERT_EQ(ex.message(), GetParam().expectedMessage);
        }
    }

    INSTANTIATE_TEST_SUITE_P(PackConfigurationTest, PackConfigurationJsonIssuesTest, testing::Values(
                                 PackConfigurationJsonIssuesParam{"", "illegal value at offset 0"},
                                 PackConfigurationJsonIssuesParam{"[]", "The json must contain an object"},
                                 PackConfigurationJsonIssuesParam{"ghkjk", "illegal value at offset 1"},
                                 PackConfigurationJsonIssuesParam{"{\"compress\":{\"include\":[\"[[\"]}}",
                                 "The regular expression \"[[\" is invalid: missing terminating ] for character class"
                                 }
                             ));

    TEST(PackConfigurationTest, Apply_Throws_If_Json_Is_Not_Object) {
        QTemporaryFile json;
        json.open();
        json.write(QByteArray(
            "{\"headers\":[{\"name\":\"p1\", \"value\":\"v1\"}, {\"name\":\"p2\", \"value\":\"v2\"}]}"));
        json.close();

        PboDocument document("file.pbo");
        PboNode* pboJson = document.root()->createHierarchy(PboPath({"pbo.json"}));
        pboJson->binarySource = QSharedPointer<BinarySource>(new io::FsRawBinarySource(json.fileName()));

        const PackConfiguration packConfiguration(&document);
        packConfiguration.apply();

        ASSERT_EQ(document.headers()->count(), 2);
        ASSERT_EQ(document.headers()->at(0)->name(), "p1");
        ASSERT_EQ(document.headers()->at(0)->value(), "v1");
        ASSERT_EQ(document.headers()->at(1)->name(), "p2");
        ASSERT_EQ(document.headers()->at(1)->value(), "v2");
    }
}
