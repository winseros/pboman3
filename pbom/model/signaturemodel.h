#pragma once

#include <QString>

namespace pboman3 {
    class SignatureModel {
    public:
        void setSignatureBytes(const QByteArray& bytes);

        const QString& signatureString() const;

    private:
        QString signatureString_;
    };
}
