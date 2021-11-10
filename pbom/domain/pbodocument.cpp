#include "pbodocument.h"
#include "validationexception.h"

namespace pboman3::domain {
    PboDocument::PboDocument(QString pboName) {
        headers_ = QSharedPointer<DocumentHeaders>(new DocumentHeaders);
        root_ = QSharedPointer<DocumentNode>(new DocumentNode(std::move(pboName), PboNodeType::Container, nullptr));
    }

    DocumentHeaders* PboDocument::headers() const {
        return headers_.get();
    }

    DocumentNode* PboDocument::root() const {
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
}
