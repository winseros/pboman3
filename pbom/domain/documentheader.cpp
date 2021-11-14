#include "documentheader.h"
#include "validationexception.h"

namespace pboman3::domain {
    DocumentHeader::DocumentHeader(QString name, QString value)
        : name_(std::move(name)),
          value_(std::move(value)) {
        validateName(name_);
    }

    DocumentHeader::DocumentHeader(const InternalData& data)
        : name_(data.name),
          value_(data.value) {
        //this is a repository constructor
        //it does no validation but must be used only from the persistence layer
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
        if (name.isEmpty())
            throw ValidationException("The name can not be empty");
    }

    bool operator==(const DocumentHeader& h1, const DocumentHeader& h2) {
        return h1.name_ == h2.name_ && h1.value_ == h2.value_;
    }

    bool operator!=(const DocumentHeader& h1, const DocumentHeader& h2) {
        return !(h1 == h2);
    }
}
