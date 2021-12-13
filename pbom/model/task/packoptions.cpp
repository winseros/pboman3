#include "packoptions.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

namespace pboman3::model::task {

    void CompressOptions::inflate(const QString& path, const QJsonObject& json) {
        include = JsonArray<JsonValue<QString>>().settle(json, path, "include", JsonMandatory::No).data();
        exclude = JsonArray<JsonValue<QString>>().settle(json, path, "exclude", JsonMandatory::No).data();
    }

    void CompressOptions::serialize(QJsonObject& target) const {
        target["include"] = JsonArray<QString>::makeJson(include);
        target["exclude"] = JsonArray<QString>::makeJson(exclude);
    }

    QDebug operator<<(QDebug debug, const CompressOptions& options) {
        return debug << "(Include=" << options.include << ", Exclude=" << options.exclude << ")";
    }

    PackHeader::PackHeader() = default;

    PackHeader::PackHeader(QString name, QString value)
        : name(std::move(name)),
          value(std::move(value)) {
    }

    void PackHeader::inflate(const QString& path, const QJsonObject& json) {
        name = JsonValue<QString>().settle(json, path, "name").value();
        if (name.isEmpty()) {
            throw JsonStructureException(path + ".name must not be an empty string");
        }
        value = JsonValue<QString>().settle(json, path, "value").value();
    }

    void PackHeader::serialize(QJsonObject& target) const {
        target["name"] = QJsonValue(name);
        target["value"] = QJsonValue(value);
    }

    QDebug operator<<(QDebug debug, const PackHeader& header) {
        return debug << "(Name=" << header.name << ", Value=" << header.value << ")";
    }

    void PackOptions::inflate(const QString& path, const QJsonObject& json) {
        headers = JsonArray<PackHeader>().settle(json, path, "headers", JsonMandatory::No).data();
        compress.settle(json, path, "compress", JsonMandatory::No);
    }

    void PackOptions::serialize(QJsonObject& target) const {
        target["headers"] = JsonArray<PackHeader>::makeJson(headers);
        target["compress"] = QJsonValue(compress.makeJson());
    }

    QDebug operator<<(QDebug debug, const PackOptions& options) {
        return debug << "(Headers=" << options.headers << ", Compress=" << options.compress << ")";
    }
}
