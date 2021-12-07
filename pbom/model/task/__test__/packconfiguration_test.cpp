#include "model/task/packconfiguration.h"
#include <QTemporaryFile>
#include <gtest/gtest.h>
#include "domain/pbodocument.h"
#include "io/bs/fslzhbinarysource.h"
#include "io/bs/fsrawbinarysource.h"

namespace pboman3::model::task::test {
    using namespace domain;

    TEST(PackConfigurationTest, Apply_Removes_All_Config_Nodes) {
        QTemporaryFile json;
        json.open();
        json.write(QByteArray("{}"));
        json.close();

        PboDocument document("file.pbo");
        document.root()->createHierarchy(PboPath({"f1.txt"}));
        PboNode* pboJson = document.root()->createHierarchy(PboPath({"pbo.json"}));
        pboJson->binarySource = QSharedPointer<BinarySource>(new io::FsRawBinarySource(json.fileName()));
        PboNode* prefix = document.root()->createHierarchy(PboPath({"$prefix$"}));
        prefix->binarySource = QSharedPointer<BinarySource>(new io::FsRawBinarySource(json.fileName()));
        PboNode* version = document.root()->createHierarchy(PboPath({"$version$"}));
        version->binarySource = QSharedPointer<BinarySource>(new io::FsRawBinarySource(json.fileName()));
        PboNode* product = document.root()->createHierarchy(PboPath({"$product$"}));
        product->binarySource = QSharedPointer<BinarySource>(new io::FsRawBinarySource(json.fileName()));

        const PackConfiguration packConfiguration(&document);
        packConfiguration.apply();

        ASSERT_EQ(document.root()->count(), 1); //config files removed
        ASSERT_TRUE(document.root()->get(PboPath({"f1.txt"}))); //but others are in places
    }

    TEST(PackConfigurationTest, Apply_Sets_Headers_From_PboJson) {
        QTemporaryFile json;
        json.open();
        json.write(QByteArray(
            "{\"headers\":[{\"name\":\"p1\", \"value\":\"v1\"}, {\"name\":\"p2\", \"value\":\"v2\"}]}"));
        json.close();

        PboDocument document("file.pbo");
        //this must be applied
        PboNode* pboJson = document.root()->createHierarchy(PboPath({"pbo.json"}));
        pboJson->binarySource = QSharedPointer<BinarySource>(new io::FsRawBinarySource(json.fileName()));
        //these must not be applied
        PboNode* prefix = document.root()->createHierarchy(PboPath({"$prefix$"}));
        prefix->binarySource = QSharedPointer<BinarySource>(new io::FsRawBinarySource(json.fileName()));
        PboNode* version = document.root()->createHierarchy(PboPath({"$version$"}));
        version->binarySource = QSharedPointer<BinarySource>(new io::FsRawBinarySource(json.fileName()));
        PboNode* product = document.root()->createHierarchy(PboPath({"$product$"}));
        product->binarySource = QSharedPointer<BinarySource>(new io::FsRawBinarySource(json.fileName()));

        const PackConfiguration packConfiguration(&document);
        packConfiguration.apply();

        ASSERT_EQ(document.headers()->count(), 2);
        ASSERT_EQ(document.headers()->at(0)->name(), "p1");
        ASSERT_EQ(document.headers()->at(0)->value(), "v1");
        ASSERT_EQ(document.headers()->at(1)->name(), "p2");
        ASSERT_EQ(document.headers()->at(1)->value(), "v2");
    }

    TEST(PackConfigurationTest, Apply_Sets_Headers_From_Prefix_Files) {
        QTemporaryFile pref;
        pref.open();
        pref.write(QByteArray("pref1"));
        pref.close();

        QTemporaryFile prod;
        prod.open();
        prod.write(QByteArray("prod1"));
        prod.close();

        QTemporaryFile ver;
        ver.open();
        ver.write(QByteArray("ver1"));
        ver.close();

        PboDocument document("file.pbo");
        PboNode* prefix = document.root()->createHierarchy(PboPath({"$prEfix$"}));
        prefix->binarySource = QSharedPointer<BinarySource>(new io::FsRawBinarySource(pref.fileName()));
        PboNode* product = document.root()->createHierarchy(PboPath({"$prOduct$"}));
        product->binarySource = QSharedPointer<BinarySource>(new io::FsRawBinarySource(prod.fileName()));
        PboNode* version = document.root()->createHierarchy(PboPath({"$veRsion$"}));
        version->binarySource = QSharedPointer<BinarySource>(new io::FsRawBinarySource(ver.fileName()));

        const PackConfiguration packConfiguration(&document);
        packConfiguration.apply();

        ASSERT_EQ(document.headers()->count(), 3);
        ASSERT_EQ(document.headers()->at(0)->name(), "prefix");
        ASSERT_EQ(document.headers()->at(0)->value(), "pref1");
        ASSERT_EQ(document.headers()->at(1)->name(), "product");
        ASSERT_EQ(document.headers()->at(1)->value(), "prod1");
        ASSERT_EQ(document.headers()->at(2)->name(), "version");
        ASSERT_EQ(document.headers()->at(2)->value(), "ver1");
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

    TEST(PackConfigurationTest, Apply_Throws_If_Prefix_Files_Non_Text) {
        constexpr char data[]{0x01, 0x02, 0x03, 0x04, 0x00, 0x01};
        QTemporaryFile pref;
        pref.open();
        pref.write(data, sizeof data);
        pref.close();

        PboDocument document("file.pbo");
        PboNode* prefix = document.root()->createHierarchy(PboPath({"$prefix$"}));
        prefix->binarySource = QSharedPointer<BinarySource>(new io::FsRawBinarySource(pref.fileName()));

        try {
            const PackConfiguration packConfiguration(&document);
            packConfiguration.apply();
            FAIL() << "Should have not reached this line";
        } catch (const PrefixEncodingException& ex) {
            ASSERT_EQ(ex.message(), "$prefix$");
        }
    }
}
