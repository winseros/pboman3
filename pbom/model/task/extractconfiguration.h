#pragma once

#include "packoptions.h"
#include "domain/pbodocument.h"

namespace pboman3::model::task {
    using namespace domain;

    class ExtractConfiguration {
    public:
        static PackOptions extractFrom(const PboDocument& document);

    private:
        static void extractHeaders(const PboDocument& document, PackOptions& options);

        static void extractCompressionRules(const PboDocument& document, PackOptions& options);

        static void collectValuableArtifacts(const PboNode* node, QSet<QString>& results);

        static QString makeExtensionCompressionRule(const QString& ext);

        static QString makeFileCompressionRule(const QString& fileName);
    };
}
