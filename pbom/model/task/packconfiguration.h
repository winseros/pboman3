#pragma once

#include "domain/pbodocument.h"
#include "packoptions.h"

namespace pboman3::model::task {
    using namespace domain;

    class PrefixEncodingException : public AppException {
    public:
        PrefixEncodingException(QString prefixFileName);

        void raise() const override;

        QException* clone() const override;

        friend QDebug& operator<<(QDebug& debug, const PrefixEncodingException& ex);
    };

    class PackConfiguration {
    public:
        explicit PackConfiguration(PboDocument* document);

        void apply() const;

    private:
        struct CompressionRules;

        PboDocument* document_;

        void applyDocumentHeaders(const PackOptions& options) const;

        void applyDocumentCompressionRules(PboNode* node, const CompressionRules& rules) const;

        static bool shouldCompress(const PboNode* node, const CompressionRules& rules);

        static CompressionRules buildCompressionRules(const PackOptions& options);

        static void convertToCompressionRules(const QList<QString>& source, QList<QRegularExpression>* dest);

        static PackOptions readPackOptions(const PboNode* node);

        static QByteArray readNodeContent(const PboNode* node);

        struct CompressionRules {
            QList<QRegularExpression> include;
            QList<QRegularExpression> exclude;
        };

        void applyNodeContentAsHeader(const PboNode* node, const QString& prefix) const;

        static void throwIfBreaksPbo(const PboNode* node, const QByteArray& data);
    };
}
