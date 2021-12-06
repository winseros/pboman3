#include "packoptions.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

namespace pboman3::model::task {
    const QList<QString>& CompressOptions::include() const {
        return include_;
    }

    const QList<QString>& CompressOptions::exclude() const {
        return exclude_;
    }

    void CompressOptions::inflate(const QString& path, const QJsonObject& json) {
        include_ = JsonArray<JsonValue<QString>>().settle(json, path, "include", JsonMandatory::No).data();
        exclude_ = JsonArray<JsonValue<QString>>().settle(json, path, "exclude", JsonMandatory::No).data();
    }


    const QString& PackHeader::name() const {
        return name_;
    }

    const QString& PackHeader::value() const {
        return value_;
    }

    void PackHeader::inflate(const QString& path, const QJsonObject& json) {
        name_ = JsonValue<QString>().settle(json, path, "name").value();
        value_ = JsonValue<QString>().settle(json, path, "value").value();
    }


    const QList<PackHeader>& PackOptions::headers() const {
        return headers_;
    }

    const CompressOptions& PackOptions::compress() const {
        return compress_;
    }

    void PackOptions::inflate(const QString& path, const QJsonObject& json) {
        headers_ = JsonArray<PackHeader>().settle(json, path, "headers", JsonMandatory::No).data();
        compress_.settle(json, path, "compress", JsonMandatory::No);
    }
}
