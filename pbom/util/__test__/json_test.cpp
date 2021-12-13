#include <gtest/gtest.h>
#include "util/json.h"
#include <QJsonDocument>

namespace pboman3::util::test {
    TEST(JsonValueTest, Settle_Reads_QJsonValue) {
        const QJsonDocument doc = QJsonDocument::fromJson("{\"obj\":{\"prop\":\"value1\"}}");

        JsonValue<QString> val1;
        val1.settle(doc.object()["obj"], "obj", "prop");

        ASSERT_EQ(val1.value(), "value1");
    }

    TEST(JsonValueTest, Settle_Produces_Error_If_Parent_Is_Not_Object) {
        const QJsonDocument doc = QJsonDocument::fromJson("{\"prop\":\"value1\"}");

        try {
            JsonValue<QString> val1;
            val1.settle(doc.object()["prop"], ".prop", "prop");
            FAIL() << "Should have not reached this line";
        } catch (const JsonStructureException& ex) {
            ASSERT_EQ(ex.message(), ".prop must be an {Object}");
        }
    }

    TEST(JsonValueTest, Settle_Produces_Error_If_Parent_Key_Missing) {
        const QJsonDocument doc = QJsonDocument::fromJson("{}");

        try {
            JsonValue<QString> val1;
            val1.settle(doc.object(), ".", "prop");
            FAIL() << "Should have not reached this line";
        } catch (const JsonStructureException& ex) {
            ASSERT_EQ(ex.message(), ". must contain the key \"prop\"");
        }
    }

    TEST(JsonValueTest, Settle_Does_Nothing_If_Optional_Parent_Key_Missing) {
        const QJsonDocument doc = QJsonDocument::fromJson("{}");

        const QString val = JsonValue<QString>().settle(doc.object(), ".", "prop", JsonMandatory::No).value();
        ASSERT_TRUE(val.isEmpty());
    }

    TEST(JsonValueTest, Settle_Produces_Error_If_Key_Is_Not_String) {
        const QJsonDocument doc = QJsonDocument::fromJson("{\"prop\":1}");

        try {
            JsonValue<QString> val1;
            val1.settle(doc.object(), "", "prop");
            FAIL() << "Should have not reached this line";
        } catch (const JsonStructureException& ex) {
            ASSERT_EQ(ex.message(), ".prop must be a {String}");
        }
    }


    class MockJsonObject : public JsonObject {
    public:
        MockJsonObject() = default;

        MockJsonObject(QString prop) : prop(std::move(prop)) {
        };

        QString prop;

    protected:
        void inflate(const QString& path, const QJsonObject& json) override {
            prop = JsonValue<QString>().settle(json, path, "prop").value();
        }

        void serialize(QJsonObject& target) const override {
            target["prop"] = QJsonValue(prop);
        }
    };

    TEST(JsonObjectTest, Settle_Reads_QJsonValue) {
        const QJsonDocument doc = QJsonDocument::fromJson("{\"obj\":{\"prop\":\"value1\"}}");

        MockJsonObject obj;
        obj.settle(doc.object(), ".obj", "obj");

        ASSERT_EQ(obj.prop, "value1");
    }

    TEST(JsonObjectTest, Settle_Produces_Error_If_Parent_Is_Not_Object) {
        const QJsonDocument doc = QJsonDocument::fromJson("{\"obj\":111}");

        try {
            MockJsonObject obj;
            obj.settle(doc.object()["obj"], ".obj", "prop");
            FAIL() << "Should have not reached this line";
        } catch (const JsonStructureException& ex) {
            ASSERT_EQ(ex.message(), ".obj must be an {Object}");
        }
    }

    TEST(JsonObjectTest, Settle_Produces_Error_If_Parent_Key_Missing) {
        const QJsonDocument doc = QJsonDocument::fromJson("{\"obj\":{}}");

        try {
            MockJsonObject obj;
            obj.settle(doc.object()["obj"], ".obj", "prop");
            FAIL() << "Should have not reached this line";
        } catch (const JsonStructureException& ex) {
            ASSERT_EQ(ex.message(), ".obj must contain the key \"prop\"");
        }
    }

    TEST(JsonObjectTest, Settle_Does_Nothing_If_Optional_Parent_Key_Missing) {
        const QJsonDocument doc = QJsonDocument::fromJson("{\"obj\":{}}");

        MockJsonObject obj;
        obj.settle(doc.object()["obj"], ".obj", "prop", JsonMandatory::No);

        ASSERT_TRUE(obj.prop.isEmpty());
    }

    TEST(JsonObjectTest, Settle_Produces_Error_If_Json_Is_Not_Object) {
        const QJsonDocument doc = QJsonDocument::fromJson("{\"obj\":{\"prop\": 11}}");

        try {
            MockJsonObject obj;
            obj.settle(doc.object()["obj"], ".obj", "prop");
            FAIL() << "Should have not reached this line";
        } catch (const JsonStructureException& ex) {
            ASSERT_EQ(ex.message(), ".obj.prop must be an {Object}");
        }
    }


    TEST(JsonArrayTest, Settle_Reads_QJsonValue_String) {
        const QJsonDocument doc = QJsonDocument::fromJson("{\"obj\":[\"s1\",\"s2\",\"s3\"]}");

        const QList<QString> array = JsonArray<JsonValue<QString>>().settle(doc.object(), ".", "obj").data();

        ASSERT_EQ(array.count(), 3);
        ASSERT_EQ(array.at(0), "s1");
        ASSERT_EQ(array.at(1), "s2");
        ASSERT_EQ(array.at(2), "s3");
    }

    TEST(JsonArrayTest, Settle_Reads_QJsonValue_Object) {
        const QJsonDocument doc = QJsonDocument::fromJson("{\"obj\":[{\"prop\":\"v1\"}, {\"prop\":\"v2\"}]}");

        JsonArray<MockJsonObject> array;
        array.settle(doc.object(), ".", "obj");

        ASSERT_EQ(array.data().count(), 2);
        ASSERT_EQ(array.data().at(0).prop, "v1");
        ASSERT_EQ(array.data().at(1).prop, "v2");
    }

    TEST(JsonArrayTest, Settle_Produces_Error_If_Parent_Is_Not_Array) {
        const QJsonDocument doc = QJsonDocument::fromJson("{\"obj\":111}");

        try {
            JsonArray<JsonValue<QString>> array;
            array.settle(doc.object()["obj"], ".obj", "prop");
            FAIL() << "Should have not reached this line";
        } catch (const JsonStructureException& ex) {
            ASSERT_EQ(ex.message(), ".obj must be an {Array}");
        }
    }

    TEST(JsonArrayTest, Settle_Produces_Error_If_Parent_Key_Missing) {
        const QJsonDocument doc = QJsonDocument::fromJson("{\"obj\":{}}");

        try {
            JsonArray<JsonValue<QString>> array;
            array.settle(doc.object()["obj"], ".obj", "prop");
            FAIL() << "Should have not reached this line";
        } catch (const JsonStructureException& ex) {
            ASSERT_EQ(ex.message(), ".obj must contain the key \"prop\"");
        }
    }

    TEST(JsonArrayTest, Settle_Does_Nothing_If_Optional_Parent_Key_Missing) {
        const QJsonDocument doc = QJsonDocument::fromJson("{\"obj\":{}}");

        const QList<QString> arr = JsonArray<JsonValue<QString>>().settle(
            doc.object()["obj"], ".obj", "prop", JsonMandatory::No).data();

        ASSERT_EQ(arr.count(), 0);
    }

    TEST(JsonArrayTest, Settle_Produces_Error_If_Json_Is_Not_Array) {
        const QJsonDocument doc = QJsonDocument::fromJson("{\"obj\":{\"prop\": 111}}");

        try {
            JsonArray<JsonValue<QString>> array;
            array.settle(doc.object()["obj"], ".obj", "prop");
            FAIL() << "Should have not reached this line";
        } catch (const JsonStructureException& ex) {
            ASSERT_EQ(ex.message(), ".obj.prop must be an {Array}");
        }
    }

    TEST(JsonArrayTest, Settle_Produces_Error_If_Element_Is_Wrong) {
        const QJsonDocument doc = QJsonDocument::fromJson("{\"obj\":[\"aa\", 11]}");

        try {
            JsonArray<JsonValue<QString>> array;
            array.settle(doc.object(), "", "obj");
            FAIL() << "Should have not reached this line";
        } catch (const JsonStructureException& ex) {
            ASSERT_EQ(ex.message(), ".obj[1] must be a {String}");
        }
    }

    TEST(JsonArrayTest, MakeJson_Writes_Array_Of_Strings) {
        const QList<QString> data{"s1", "s2", "s3"};
        const QJsonArray json = JsonArray<QString>::makeJson(data);

        ASSERT_EQ(json.count(), 3);
        ASSERT_EQ(json.at(0).toString(), "s1");
        ASSERT_EQ(json.at(1).toString(), "s2");
        ASSERT_EQ(json.at(2).toString(), "s3");
    }

    TEST(JsonArrayTest, MakeJson_Writes_Array_Of_Objects) {
        const QList data{MockJsonObject("p1"), MockJsonObject("p2")};
        const QJsonArray json = JsonArray<MockJsonObject>::makeJson(data);

        ASSERT_EQ(json.count(), 2);
        ASSERT_EQ(json.at(0).toObject()["prop"], "p1");
        ASSERT_EQ(json.at(1).toObject()["prop"], "p2");
    }
}
