#include "pbojson.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

namespace pboman3::model::task {

    void PboJsonCompressOptions::inflate(const QString& path, const QJsonObject& json) {
        include = JsonArray<JsonValue<QString>>().settle(json, path, "include", JsonMandatory::No).data();
        exclude = JsonArray<JsonValue<QString>>().settle(json, path, "exclude", JsonMandatory::No).data();
    }

    void PboJsonCompressOptions::serialize(QJsonObject& target) const {
        target["include"] = JsonArray<QString>::makeJson(include);
        target["exclude"] = JsonArray<QString>::makeJson(exclude);
    }

    QDebug operator<<(QDebug debug, const PboJsonCompressOptions& options) {
        return debug << "(Include=" << options.include << ", Exclude=" << options.exclude << ")";
    }

    PboJsonHeader::PboJsonHeader() = default;

    PboJsonHeader::PboJsonHeader(QString name, QString value)
        : name(std::move(name)),
          value(std::move(value)) {
    }

    void PboJsonHeader::inflate(const QString& path, const QJsonObject& json) {
        name = JsonValue<QString>().settle(json, path, "name").value();
        if (name.isEmpty()) {
            throw JsonStructureException(path + ".name must not be an empty string");
        }
        value = JsonValue<QString>().settle(json, path, "value").value();
    }

    void PboJsonHeader::serialize(QJsonObject& target) const {
        target["name"] = QJsonValue(name);
        target["value"] = QJsonValue(value);
    }

    QDebug operator<<(QDebug debug, const PboJsonHeader& header) {
        return debug << "(Name=" << header.name << ", Value=" << header.value << ")";
    }

    void PboJson::inflate(const QString& path, const QJsonObject& json) {
        headers = JsonArray<PboJsonHeader>().settle(json, path, "headers", JsonMandatory::No).data();
        compress.settle(json, path, "compress", JsonMandatory::No);
    }

    void PboJson::serialize(QJsonObject& target) const {
        target["headers"] = JsonArray<PboJsonHeader>::makeJson(headers);
        target["compress"] = QJsonValue(compress.makeJson());
    }

    QDebug operator<<(QDebug debug, const PboJson& options) {
        return debug << "(Headers=" << options.headers << ", Compress=" << options.compress << ")";
    }
}
