#include "pbonodetransaction.h"
#include "validationexception.h"
#include "exception.h"
#include "domain/func.h"

namespace pboman3::domain {
    PboNodeTransaction::PboNodeTransaction(PboNode* node)
        : committed_(false),
        node_(node),
        title_(node->title()) {
    }

    PboNodeTransaction::~PboNodeTransaction() {
        if (committed_)
            node_->setTitle(std::move(title_));
    }

    const QString& PboNodeTransaction::title() const {
        return title_;
    }

    void PboNodeTransaction::setTitle(QString title) {
        if (committed_)
            throw InvalidOperationException("The transaction has already committed");

        if (title != title_) {
            if (const QString err = verifyTitle(title); !err.isEmpty())
                throw ValidationException(err);
        }

        title_ = std::move(title);
    }

    void PboNodeTransaction::commit() {
        committed_ = true;
    }

    QString PboNodeTransaction::verifyTitle(const QString& title) const {
        if (title.isEmpty())
            return "The value can not be empty";
        if (!node_->parentNode_)
            return "";
        const PboNode* existing = node_->parentNode_->get(PboPath{title});
        return existing && existing != node_ ? "The item with this name already exists" : "";
    }
}
