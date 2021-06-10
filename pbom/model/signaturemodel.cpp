#include "signaturemodel.h"

namespace pboman3 {
    void SignatureModel::setSignatureBytes(const QByteArray& bytes) {
        if (bytes.count() == 0)
            signatureString_.clear();
        else
            signatureString_ = bytes.toHex(' ');
    }

    const QString& SignatureModel::signatureString() const {
        return signatureString_;
    }
}
