#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include "exception.h"

namespace pboman3::util {
    enum class JsonMandatory {
        Yes,
        No
    };

    class JsonStructureException : public AppException {
    public:
        JsonStructureException(QString message);

        friend QDebug operator<<(QDebug debug, const JsonStructureException& ex);

        void raise() const override;

        QException* clone() const override;
    };

    namespace json {
        template <typename T>
        struct JsonValueTrait {
            static constexpr auto typeName = "<Developer must override this>";

            static T read(const QJsonValue& value);
        };

        template <>
        struct JsonValueTrait<QString> {
            static constexpr auto typeName = "{String}";

            static QString read(const QJsonValue& value) {
                return value.toString();
            }
        };
    }

    template <typename T>
    class JsonValue {
    public:
        T& value() {
            return value_;
        }

        JsonValue<T>& settle(const QJsonValue& value, const QString& path) {
            if (!value.isString()) {
                throw JsonStructureException(path + " must be a " + json::JsonValueTrait<T>::typeName);
            }
            value_ = json::JsonValueTrait<T>::read(value);
            return *this;
        }

        JsonValue<T>& settle(const QJsonObject& parent, const QString& parentPath, const QString& name,
                             JsonMandatory mandatory = JsonMandatory::Yes) {
            if (!parent.contains(name)) {
                if (mandatory == JsonMandatory::No)
                    return *this;
                throw JsonStructureException(parentPath + " must contain the key \"" + name + "\"");
            }

            return settle(parent[name], parentPath + "." + name);
        }

        JsonValue<T>& settle(const QJsonValue& parent, const QString& parentPath, const QString& name,
                             JsonMandatory mandatory = JsonMandatory::Yes) {
            if (!parent.isObject()) {
                throw JsonStructureException(parentPath + " must be an {Object}");
            }
            return settle(parent.toObject(), parentPath, name, mandatory);
        }

    private:
        T value_;
    };

    class JsonObject {
    public:
        virtual ~JsonObject() = default;

        void settle(const QJsonObject& json, const QString& path);

        void settle(const QJsonValue& json, const QString& path);

        void settle(const QJsonObject& parent, const QString& parentPath, const QString& name, JsonMandatory mandatory = JsonMandatory::Yes);

        void settle(const QJsonValue& parent, const QString& parentPath, const QString& name, JsonMandatory mandatory = JsonMandatory::Yes);

        QJsonObject makeJson() const;

    protected:
        virtual void inflate(const QString& path, const QJsonObject& json) = 0;

        virtual void serialize(QJsonObject& target) const = 0;
    };

    namespace json {
        template <typename T>
        struct JsonArrayTraits {
            using TypeOuter = T;
            using TypeInner = T;
            static TypeOuter& getValue(TypeInner& t) { return t; }
            static QJsonValue makeJson(const TypeOuter& t) {
                if constexpr (std::is_base_of_v<JsonObject, T>) {
                    return QJsonValue(t.makeJson());
                } else {
                    return QJsonValue(t);
                }
            }
        };

        template <typename T>
        struct JsonArrayTraits<JsonValue<T>> {
            using TypeOuter = T;
            using TypeInner = JsonValue<T>;
            static TypeOuter& getValue(TypeInner& t) { return t.value(); }
            static QJsonValue makeJson(const TypeOuter& t); //not implemented
        };
    }

    template <typename T>
    class JsonArray {
    public:
        QList<typename json::JsonArrayTraits<T>::TypeOuter> data() {
            return data_;
        }

        JsonArray<T>& settle(const QJsonArray& json, const QString& path) {
            inflate(path, json);
            return *this;
        }

        JsonArray<T>& settle(const QJsonValue& json, const QString& path) {
            if (!json.isArray()) {
                throw JsonStructureException(path + " must be an {Array}");
            }
            return settle(json.toArray(), path);
        }

        JsonArray<T>& settle(const QJsonObject& parent, const QString& parentPath, const QString& name,
                             JsonMandatory mandatory = JsonMandatory::Yes) {
            if (!parent.contains(name)) {
                if (mandatory == JsonMandatory::No)
                    return *this;
                throw JsonStructureException(parentPath + " must contain the key \"" + name + "\"");
            }

            return settle(parent[name], parentPath + "." + name);
        }

        JsonArray<T>& settle(const QJsonValue& parent, const QString& parentPath, const QString& name,
                             JsonMandatory mandatory = JsonMandatory::Yes) {
            if (!parent.isObject()) {
                throw JsonStructureException(parentPath + " must be an {Array}");
            }
            return settle(parent.toObject(), parentPath, name, mandatory);
        }

        static QJsonArray makeJson(const QList<typename json::JsonArrayTraits<T>::TypeOuter>& data) {
            QJsonArray result;
            for (const typename json::JsonArrayTraits<T>::TypeOuter& d : data) {
                result.append(json::JsonArrayTraits<T>::makeJson(d));
            }
            return result;
        }

    private:
        QList<typename json::JsonArrayTraits<T>::TypeOuter> data_;

        void inflate(const QString& path, const QJsonArray& json) {
            data_.reserve(json.count());
            int i = 0;
            auto it = json.begin();
            while (it != json.end()) {
                typename json::JsonArrayTraits<T>::TypeInner item;
                item.settle(*it, path + "[" + QString::number(i) + "]");
                data_.append(json::JsonArrayTraits<T>::getValue(item));
                ++it;
                i++;
            }
        }
    };
}
