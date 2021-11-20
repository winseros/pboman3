#include "documentheaders.h"
#include "documentheaderstransaction.h"
#include "exception.h"

namespace pboman3::domain {
    DocumentHeaders::DocumentHeaders() = default;

    DocumentHeaders::DocumentHeaders(QList<QSharedPointer<DocumentHeader>> headers)
        : headers_(std::move(headers)) {
        //this is a repository constructor
        //it does no validation but must be used only from the persistence layer
    }

    qsizetype DocumentHeaders::count() const {
        return headers_.count();
    }

    const DocumentHeader* DocumentHeaders::at(qsizetype index) const {
        return headers_.at(index).get();
    }

    QPointerListIterator<DocumentHeader> DocumentHeaders::begin() {
        return QPointerListIterator<DocumentHeader>(headers_.begin());
    }

    QPointerListIterator<DocumentHeader> DocumentHeaders::end() {
        return QPointerListIterator<DocumentHeader>(headers_.end());
    }

    QPointerListConstIterator<DocumentHeader> DocumentHeaders::begin() const {
        return QPointerListConstIterator<DocumentHeader>(headers_.constBegin());
    }

    QPointerListConstIterator<DocumentHeader> DocumentHeaders::end() const {
        return QPointerListConstIterator<DocumentHeader>(headers_.constEnd());
    }

    QSharedPointer<DocumentHeadersTransaction> DocumentHeaders::beginTransaction() {
        return QSharedPointer<DocumentHeadersTransaction>(new DocumentHeadersTransaction(headers_, this));
    }

    void DocumentHeaders::setHeaders(QList<QSharedPointer<DocumentHeader>> headers) {
        if (areDifferent(headers_, headers)) {
            headers_ = std::move(headers);
            emit headersChanged();
        }
    }

    bool DocumentHeaders::areDifferent(const QList<QSharedPointer<DocumentHeader>>& list1, const QList<QSharedPointer<DocumentHeader>>& list2) {
        if (list1.count() != list2.count())
            return true;

        auto it1 = list1.begin();
        auto it2 = list2.begin();

        while (it1 != list1.end()) {
            if (**it1 != **it2)
                return true;

            ++it1;
            ++it2;
        }

        return false;
    }
}
