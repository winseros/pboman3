#pragma once

#include <QDir>

#include "pbojson.h"
#include "domain/pbodocument.h"
#include "io/settings/fileconflictresolutionmode.h"

namespace pboman3::model::task {
    using namespace domain;
    using namespace io;

    class PboJsonHelper {
    public:
        static PboJson extractFrom(const PboDocument& document);

        static QString getConfigFilePath(const QDir& dir, FileConflictResolutionMode::Enum conflictResolutionMode);

        static QString allocateTempConfigFilePath();

        static void saveTo(const PboJson& options, const QString& fileName);

    private:
        static void extractHeaders(const PboDocument& document, PboJson& options);

        static void extractCompressionRules(const PboDocument& document, PboJson& options);

        static void collectValuableArtifacts(const PboNode* node, QSet<QString>& results);

        static QString makeExtensionCompressionRule(const QString& ext);

        static QString makeFileCompressionRule(const QString& fileName);
    };
}
