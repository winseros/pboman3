#pragma once

#include "documentheader.h"
#include "util/qpointerlistiterator.h"
#include <QObject>

namespace pboman3::domain {
    using namespace util;

    class DocumentHeadersTransaction;

    class DocumentHeaders : public QObject {
        Q_OBJECT

    public:
        DocumentHeaders();

        explicit DocumentHeaders(QList<QSharedPointer<DocumentHeader>> headers); //Repository Ctor

        [[nodiscard]] qsizetype count() const;

        [[nodiscard]] const DocumentHeader* at(qsizetype index) const;

        QPointerListIterator<DocumentHeader> begin();

        QPointerListIterator<DocumentHeader> end();

        [[nodiscard]] QPointerListConstIterator<DocumentHeader> begin() const;

        [[nodiscard]] QPointerListConstIterator<DocumentHeader> end() const;

        QSharedPointer<DocumentHeadersTransaction> beginTransaction();

        friend DocumentHeadersTransaction;

        friend QDebug& operator<<(QDebug& debug, const DocumentHeaders& headers);

        static const QString PREFIX_HEADER_NAME;
        
    signals:
        void headersChanged();

    private:
        QList<QSharedPointer<DocumentHeader>> headers_;

        void setHeaders(QList<QSharedPointer<DocumentHeader>> headers);

        static bool areDifferent(const QList<QSharedPointer<DocumentHeader>>& list1,
                                 const QList<QSharedPointer<DocumentHeader>>& list2);
    };
}
