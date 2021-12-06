#pragma once

#include "util/json.h"

namespace pboman3::model::task {
    using namespace util;

    class CompressOptions : public JsonObject {
    public:
        const QList<QString>& include() const;

        const QList<QString>& exclude() const;

    protected:
        void inflate(const QString& path, const QJsonObject& json) override;

    private:
        QList<QString> include_;
        QList<QString> exclude_;
    };

    class PackHeader : public JsonObject {
    public:
        const QString& name() const;

        const QString& value() const;

    protected:
        void inflate(const QString& path, const QJsonObject& json) override;

    private:
        QString name_;
        QString value_;
    };

    class PackOptions : public JsonObject {
    public:
        const QList<PackHeader>& headers() const;

        const CompressOptions& compress() const;

    protected:
        void inflate(const QString& path, const QJsonObject& json) override;

    private:
        QList<PackHeader> headers_;
        CompressOptions compress_;
    };
}
