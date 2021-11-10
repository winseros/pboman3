#pragma once

#include <QString>

namespace pboman3::domain {
    class DocumentHeader {
    public:
        DocumentHeader(QString name, QString value);

        const QString& name() const;

        void setName(const QString& name);

        const QString& value() const;

        void setValue(const QString& value);

    private:
        QString name_;
        QString value_;

        void validateName(const QString& name);
    };
}
