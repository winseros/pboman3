#pragma once

#include "documentheaders.h"
#include "pbonode.h"

namespace pboman3::domain {
    class PboDocument : public QObject {
        Q_OBJECT

    public:
        PboDocument(QString pboName);

        PboDocument(QString pboName, QList<QSharedPointer<DocumentHeader>> headers, QByteArray signature);//Repository Ctor

        DocumentHeaders* headers() const;

        PboNode* root() const;

        const QByteArray& signature() const;

        void setSignature(QByteArray signature);

    signals:
        void changed();

        void titleChanged(const QString& title);

        friend QDebug& operator<<(QDebug& debug, const PboDocument& document);

    private:
        QSharedPointer<DocumentHeaders> headers_;
        QSharedPointer<PboNode> root_;
        QByteArray signature_;

        void setupConnections();
    };
}
