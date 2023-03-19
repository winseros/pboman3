#include "util/json.h"
#include <QJsonDocument>

namespace pboman3::util {
    JsonStructureException::JsonStructureException(QString message)
        : AppException(std::move(message)) {
    }

    PBOMAN_EX_IMPL_DEFAULT(JsonStructureException)

    void JsonObject::settle(const QJsonObject& json, const QString& path) {
        inflate(path, json);
    }

    void JsonObject::settle(const QJsonValue& json, const QString& path) {
        if (!json.isObject()) {
            throw JsonStructureException(path + " must be an {Object}");
        }
        settle(json.toObject(), path);
    }

    void JsonObject::settle(const QJsonObject& parent, const QString& parentPath, const QString& name, JsonMandatory mandatory) {
        if (!parent.contains(name)) {
            if (mandatory == JsonMandatory::No)
                return;
            throw JsonStructureException(parentPath + " must contain the key \"" + name + "\"");
        }

        settle(parent[name], parentPath + "." + name);
    }

    void JsonObject::settle(const QJsonValue& parent, const QString& parentPath, const QString& name,
                            JsonMandatory mandatory) {
        if (!parent.isObject()) {
            throw JsonStructureException(parentPath + " must be an {Object}");
        }
        settle(parent.toObject(), parentPath, name, mandatory);
    }

    QJsonObject JsonObject::makeJson() const {
        QJsonObject self;
        serialize(self);
        return self;
    }
}
