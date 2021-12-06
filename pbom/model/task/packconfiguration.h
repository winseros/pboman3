#pragma once

#include "domain/pbodocument.h"
#include "packoptions.h"

namespace pboman3::model::task {
    using namespace domain;

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

        void applyFileContentAsHeader(const QString& prefix) const;
    };
}
