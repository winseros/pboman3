#include "documentheader.h"

namespace pboman3::domain {
    DocumentHeader::DocumentHeader(QString name, QString value) {
        validateName(name);
        name_ = std::move(name);
        value_ = std::move(value);
    }

    const QString& DocumentHeader::name() const {
        return name_;
    }

    void DocumentHeader::setName(const QString& name) {
        validateName(name);
        name_ = name;
    }

    const QString& DocumentHeader::value() const {
        return value_;
    }

    void DocumentHeader::setValue(const QString& value) {
        value_ = value;
    }

    void DocumentHeader::validateName(const QString& name) {
    }
}
