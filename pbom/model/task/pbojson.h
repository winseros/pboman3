#pragma once

#include "util/json.h"

namespace pboman3::model::task {
    using namespace util;

    class PboJsonCompressOptions : public JsonObject {
    public:
        QList<QString> include;
        QList<QString> exclude;

        friend QDebug operator<<(QDebug debug, const PboJsonCompressOptions& options);

    protected:
        void inflate(const QString& path, const QJsonObject& json) override;

        void serialize(QJsonObject& target) const override;
    };

    class PboJsonHeader : public JsonObject {
    public:
        PboJsonHeader();

        PboJsonHeader(QString name, QString value);

        QString name;
        QString value;

        friend QDebug operator<<(QDebug debug, const PboJsonHeader& header);

    protected:
        void inflate(const QString& path, const QJsonObject& json) override;

        void serialize(QJsonObject& target) const override;
    };

    class PboJson : public JsonObject {
    public:
        QList<PboJsonHeader> headers;
        PboJsonCompressOptions compress;

        friend QDebug operator<<(QDebug debug, const PboJson& options);

    protected:
        void inflate(const QString& path, const QJsonObject& json) override;

        void serialize(QJsonObject& target) const override;
    };
}
