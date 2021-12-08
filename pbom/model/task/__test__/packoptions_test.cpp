#include "model/task/packoptions.h"
#include <gtest/gtest.h>
#include <QJsonDocument>

namespace pboman3::model::task::test {
    TEST(PackOptionsTest, Settle_Reads_Json_With_All_Fields) {
        const QJsonDocument json = QJsonDocument::fromJson(
            "{\"headers\":[{\"name\":\"n1\",\"value\":\"v1\"},{\"name\":\"n2\",\"value\":\"v2\"}], \"compress\":{\"include\":[\"i1\",\"i2\"],\"exclude\":[\"e1\",\"e2\"]}}");

        PackOptions config;
        config.settle(json.object(), "");

        ASSERT_EQ(config.headers.count(), 2);
        ASSERT_EQ(config.headers.at(0).name, "n1");
        ASSERT_EQ(config.headers.at(0).value, "v1");
        ASSERT_EQ(config.headers.at(1).name, "n2");
        ASSERT_EQ(config.headers.at(1).value, "v2");

        ASSERT_EQ(config.compress.include.count(), 2);
        ASSERT_EQ(config.compress.include.at(0), "i1");
        ASSERT_EQ(config.compress.include.at(1), "i2");
        ASSERT_EQ(config.compress.exclude.count(), 2);
        ASSERT_EQ(config.compress.exclude.at(0), "e1");
        ASSERT_EQ(config.compress.exclude.at(1), "e2");
    }

    TEST(PackOptionsTest, Settle_Reads_Empty_Configuration) {
        const QJsonDocument json = QJsonDocument::fromJson("{}");

        PackOptions config;
        config.settle(json.object(), "");

        ASSERT_EQ(config.headers.count(), 0);
        ASSERT_EQ(config.compress.include.count(), 0);
        ASSERT_EQ(config.compress.exclude.count(), 0);
    }

    TEST(PackOptionsTest, Settle_Reads_Empty_Compression) {
        const QJsonDocument json = QJsonDocument::fromJson("{\"compression\":{}}");

        PackOptions config;
        config.settle(json.object(), "");

        ASSERT_EQ(config.headers.count(), 0);
        ASSERT_EQ(config.compress.include.count(), 0);
        ASSERT_EQ(config.compress.exclude.count(), 0);
    }

    TEST(PackOptionsTest, Settle_Throws_If_Header_Name_Is_Empty) {
        const QJsonDocument json = QJsonDocument::fromJson("{\"headers\":[{\"name\":\"\"}]}");

        try {
            PackOptions config;
            config.settle(json.object(), "");
            FAIL() << "Should have not reached this line";
        } catch (const JsonStructureException& ex) {
            ASSERT_EQ(".headers[0].name must not be an empty string", ex.message());
        }
    }
}
