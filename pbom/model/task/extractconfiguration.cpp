#include "extractconfiguration.h"
#include <QJsonDocument>
#include "domain/func.h"
#include "io/diskaccessexception.h"
#include "io/fileconflictresolutionpolicy.h"
#include "model/binarysourceutils.h"
#include "util/filenames.h"

namespace pboman3::model::task {
    PackOptions ExtractConfiguration::extractFrom(const PboDocument& document) {
        PackOptions options;

        extractHeaders(document, options);
        extractCompressionRules(document, options);

        return options;
    }

    void ExtractConfiguration::saveTo(const PackOptions& options, const QDir& dest, FileConflictResolutionMode::Enum conflictResolutionMode) {
        const QJsonObject json = options.makeJson();
        const QByteArray bytes = QJsonDocument(json).toJson(QJsonDocument::Indented);

        const QString fileName = getConfigFileName(dest, conflictResolutionMode);
        QFile file(fileName);
        if (!file.open(QIODeviceBase::WriteOnly)) {
            throw DiskAccessException("Can not access the file. Check if it is used by other processes.", fileName);
        }
        file.write(bytes);
        file.close();
    }

    void ExtractConfiguration::extractHeaders(const PboDocument& document, PackOptions& options) {
        for (const DocumentHeader* header : *document.headers()) {
            options.headers.append(PackHeader(header->name(), header->value()));
        }
    }

    constexpr const char* extensions[] = {"txt", "xml", "csv"};
    constexpr const char* files[] = {"mission.sqm", "description.ext"};

    void ExtractConfiguration::extractCompressionRules(const PboDocument& document, PackOptions& options) {
        QSet<QString> artifacts;
        artifacts.reserve(10);
        collectValuableArtifacts(document.root(), artifacts);

        for (const QString ext : extensions) {
            if (artifacts.contains(ext)) {
                const QString rule = makeExtensionCompressionRule(ext);
                options.compress.include.append(rule);
            }
        }
        for (const QString file : files) {
            if (const PboNode* node = FindDirectChild(document.root(), file); node) {
                if (IsCompressed(node->binarySource)) {
                    const QString rule = makeFileCompressionRule(file);
                    options.compress.include.append(rule);
                }
            }
        }
    }

    void ExtractConfiguration::collectValuableArtifacts(const PboNode* node, QSet<QString>& results) {
        const QString title = node->title().toLower();
        if (node->nodeType() == PboNodeType::File) {
            const QString ext = FileNames::getFileExtension(title);
            results.insert(ext);
        } else {
            for (const PboNode* child : *node) {
                collectValuableArtifacts(child, results);
            }
        }
    }

    QString ExtractConfiguration::makeExtensionCompressionRule(const QString& ext) {
        return "\\." + ext + "$";
    }

    QString ExtractConfiguration::makeFileCompressionRule(const QString& fileName) {
        return "^" + fileName + "$";
    }

    QString ExtractConfiguration::getConfigFileName(const QDir& dir, FileConflictResolutionMode::Enum conflictResolutionMode) {
        const QString configName("pbo");
        const QString configExt(".json");

        QString path = dir.absoluteFilePath(configName + configExt);
        const FileConflictResolutionPolicy policy(conflictResolutionMode);
        path = policy.resolvePotentialConflicts(path);

        return path;
    }

}
