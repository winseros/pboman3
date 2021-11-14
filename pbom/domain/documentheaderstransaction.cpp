#include "documentheaders.h"
#include "documentheaderstransaction.h"
#include "util/exception.h"

namespace pboman3::domain {
    DocumentHeadersTransaction::DocumentHeadersTransaction(QList<QSharedPointer<DocumentHeader>> headers, DocumentHeaders* parent)
        : committed_(false),
        headers_(std::move(headers)),
        parent_(parent) {
    }

    DocumentHeadersTransaction::~DocumentHeadersTransaction() {
        if (committed_) {
            parent_->setHeaders(std::move(headers_));
        }
    }

    void DocumentHeadersTransaction::commit() {
        committed_ = true;
    }

    qsizetype DocumentHeadersTransaction::count() const {
        return headers_.count();
    }

    const DocumentHeader* DocumentHeadersTransaction::at(qsizetype index) const {
        return headers_.at(index).data();
    }

    void DocumentHeadersTransaction::add(const QString& name, const QString& value) {
        throwIfCommitted();
        const QSharedPointer<DocumentHeader> header(new DocumentHeader(name, value));
        headers_.append(header);
    }

    void DocumentHeadersTransaction::clear() {
        throwIfCommitted();
        headers_.clear();
    }

    QPointerListIterator<DocumentHeader> DocumentHeadersTransaction::begin() {
        return QPointerListIterator<DocumentHeader>(headers_.begin());
    }

    QPointerListIterator<DocumentHeader> DocumentHeadersTransaction::end() {
        return QPointerListIterator<DocumentHeader>(headers_.end());
    }

    void DocumentHeadersTransaction::throwIfCommitted() const {
        if (committed_) throw InvalidOperationException("You must not modify a committed transaction.");
    }
}
