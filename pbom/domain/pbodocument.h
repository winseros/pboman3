#pragma once

#include "documentheaders.h"
#include "documentnode.h"

namespace pboman3::domain {
    class PboDocument : public QObject {
        Q_OBJECT

    public:
        PboDocument(QString pboName);

        DocumentHeaders* headers() const;

        DocumentNode* root() const;

        const QByteArray& signature() const;

        void setSignature(QByteArray signature);

    private:
        QSharedPointer<DocumentHeaders> headers_;
        QSharedPointer<DocumentNode> root_;
        QByteArray signature_;
    };
}
