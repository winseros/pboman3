#pragma once

#include "documentheader.h"
#include "util/qpointerlistiterator.h"

namespace pboman3::domain {
    class DocumentHeaders : public QObject {
        Q_OBJECT

    public:
        qsizetype count() const;

        const DocumentHeader* at(qsizetype index) const;

        void add(const QString& name, const QString& value);

        void remove(const DocumentHeader* header);

        void moveUp(const DocumentHeader* header);

        void moveDown(const DocumentHeader* header);

        QPointerListIterator<DocumentHeader> begin();

        QPointerListIterator<DocumentHeader> end();

        QPointerListConstIterator<DocumentHeader> begin() const;

        QPointerListConstIterator<DocumentHeader> end() const;

    signals:
        void headerAdded(const DocumentHeader* header, qsizetype index);

        void headerRemoved(const DocumentHeader* header, qsizetype index);

        void headerMoved(const DocumentHeader* header, qsizetype prevIndex, qsizetype newIndex);

    private:
        QList<QSharedPointer<DocumentHeader>> headers_;

        qsizetype getIndex(const DocumentHeader* header) const;
    };
}
