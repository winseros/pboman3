#pragma once

#include <QString>

namespace pboman3::domain {
    class DocumentHeader {
    public:
        DocumentHeader(QString name, QString value);

        struct InternalData {
            QString name;
            QString value;
        };

        explicit DocumentHeader(const InternalData& data);//Repository Ctor

        [[nodiscard]] const QString& name() const;

        void setName(const QString& name);

        [[nodiscard]] const QString& value() const;

        void setValue(const QString& value);

        friend bool operator==(const DocumentHeader& h1, const DocumentHeader& h2);

        friend bool operator!=(const DocumentHeader& h1, const DocumentHeader& h2);

    private:
        QString name_;
        QString value_;

        void validateName(const QString& name);
    };
}
