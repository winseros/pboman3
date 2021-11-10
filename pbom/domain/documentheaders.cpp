#include "documentheaders.h"
#include "util/exception.h"

namespace pboman3::domain {
    qsizetype DocumentHeaders::count() const {
        return headers_.count();
    }

    const DocumentHeader* DocumentHeaders::at(qsizetype index) const {
        return headers_.at(index).get();
    }

    void DocumentHeaders::add(const QString& name, const QString& value) {
        const QSharedPointer<DocumentHeader> header(new DocumentHeader(name, value));
        headers_.append(header);
        emit headerAdded(header.get(), headers_.count() - 1);
    }

    void DocumentHeaders::remove(const DocumentHeader* header) {
        const qsizetype index = getIndex(header);
        headers_.removeAt(index);
        emit headerRemoved(header, index);
    }

    void DocumentHeaders::moveUp(const DocumentHeader* header) {
        const qsizetype currentIndex = getIndex(header);
        if (currentIndex == 0)
            throw InvalidOperationException("The header is already at the top of the list");

        const qsizetype newIndex = currentIndex - 1;
        headers_.move(currentIndex, newIndex);
    }

    void DocumentHeaders::moveDown(const DocumentHeader* header) {
        const qsizetype currentIndex = getIndex(header);
        const qsizetype newIndex = currentIndex + 1;
        if (newIndex >= count())
            throw InvalidOperationException("The header is already at the bottom of the list");

        headers_.move(currentIndex, newIndex);
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

    qsizetype DocumentHeaders::getIndex(const DocumentHeader* header) const {
        qsizetype index = 0;
        for (const QSharedPointer<DocumentHeader>& h : headers_) {
            if (h.get() == header)
                return index;
            index++;
        }
        throw InvalidOperationException("The header does not belong to this instance");
    }
}
