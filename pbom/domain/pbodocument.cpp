#include "pbodocument.h"
#include "validationexception.h"

namespace pboman3::domain {
    PboDocument::PboDocument(QString pboName) {
        headers_ = QSharedPointer<DocumentHeaders>(new DocumentHeaders);
        root_ = QSharedPointer<PboNode>(new PboNode(std::move(pboName), PboNodeType::Container, nullptr));
        setupConnections();
    }

    PboDocument::PboDocument(QString pboName, QList<QSharedPointer<DocumentHeader>> headers, QByteArray signature)
        : signature_(std::move(signature)) {
        root_ = QSharedPointer<PboNode>(new PboNode(std::move(pboName), PboNodeType::Container, nullptr));
        headers_ = QSharedPointer<DocumentHeaders>(new DocumentHeaders(std::move(headers)));
        setupConnections();
        //this is a repository constructor
        //it does no validation but must be used only from the persistence layer
    }

    DocumentHeaders* PboDocument::headers() const {
        return headers_.get();
    }

    PboNode* PboDocument::root() const {
        return root_.get();
    }

    const QByteArray& PboDocument::signature() const {
        return signature_;
    }

    void PboDocument::setSignature(QByteArray signature) {
        if (signature.length() != 20)
            throw ValidationException("The signature must be 20 bytes long");
        signature_ = std::move(signature);
    }

    void PboDocument::setupConnections() {
        connect(root_.get(), &PboNode::hierarchyChanged, [this] {
            emit changed();
        });

        connect(root_.get(), &PboNode::titleChanged, [this](const QString& title) {
            emit titleChanged(title);
        });

        connect(headers_.get(), &DocumentHeaders::headersChanged, [this]() {
            emit changed();
        });
    }

    QDebug& operator<<(QDebug& debug, const PboDocument& document) {
        return debug << "PboDocument(Headers=" << *document.headers_  << ", Root=" << *document.root_ << ", Signature="
            << document.signature_.length() << "bytes)";
    }
}
