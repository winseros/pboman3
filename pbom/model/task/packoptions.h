#pragma once

#include "util/json.h"

namespace pboman3::model::task {
    using namespace util;

    class CompressOptions : public JsonObject {
    public:
        QList<QString> include;
        QList<QString> exclude;

        friend QDebug operator<<(QDebug debug, const CompressOptions& options);

    protected:
        void inflate(const QString& path, const QJsonObject& json) override;

        void serialize(QJsonObject& target) const override;
    };

    class PackHeader : public JsonObject {
    public:
        PackHeader();

        PackHeader(QString name, QString value);

        QString name;
        QString value;

        friend QDebug operator<<(QDebug debug, const PackHeader& header);

    protected:
        void inflate(const QString& path, const QJsonObject& json) override;

        void serialize(QJsonObject& target) const override;
    };

    class PackOptions : public JsonObject {
    public:
        QList<PackHeader> headers;
        CompressOptions compress;

        friend QDebug operator<<(QDebug debug, const PackOptions& options);

    protected:
        void inflate(const QString& path, const QJsonObject& json) override;

        void serialize(QJsonObject& target) const override;
    };
}
