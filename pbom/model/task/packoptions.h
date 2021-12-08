#pragma once

#include "util/json.h"

namespace pboman3::model::task {
    using namespace util;

    class CompressOptions : public JsonObject {
    public:
        QList<QString> include;
        QList<QString> exclude;

    protected:
        void inflate(const QString& path, const QJsonObject& json) override;
    };

    class PackHeader : public JsonObject {
    public:
        PackHeader();

        PackHeader(QString name, QString value);

        QString name;
        QString value;

    protected:
        void inflate(const QString& path, const QJsonObject& json) override;
    };

    class PackOptions : public JsonObject {
    public:
        QList<PackHeader> headers;
        CompressOptions compress;

    protected:
        void inflate(const QString& path, const QJsonObject& json) override;
    };
}
