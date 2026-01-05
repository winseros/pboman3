#pragma once

#include <QSharedPointer>
#include <QList>
#include "domain/documentheader.h"
#include "domain/documentheaders.h"

namespace pboman3::domain {
    using namespace util;

    class DocumentHeadersTransaction : public QObject {
        Q_OBJECT

    public:
        explicit DocumentHeadersTransaction(QList<QSharedPointer<DocumentHeader>> headers, DocumentHeaders* parent);

        ~DocumentHeadersTransaction() override;

        [[nodiscard]] qsizetype count() const;

        [[nodiscard]] const DocumentHeader* at(qsizetype index) const;

        void add(const QString& name, const QString& value);

        void clear();

        QPointerListIterator<DocumentHeader> begin();

        QPointerListIterator<DocumentHeader> end();

        void commit();

    private:
        bool committed_;
        QList<QSharedPointer<DocumentHeader>> headers_;
        DocumentHeaders* parent_;

        void throwIfCommitted() const;
    };
}
