#include "pbojsonhelper.h"
#include <QJsonDocument>
#include "domain/func.h"
#include "io/diskaccessexception.h"
#include "io/fileconflictresolutionpolicy.h"
#include "model/binarysourceutils.h"
#include "util/filenames.h"

namespace pboman3::model::task {
    PboJson PboJsonHelper::extractFrom(const PboDocument& document) {
        PboJson options;

        extractHeaders(document, options);
        extractCompressionRules(document, options);

        return options;
    }

    QString PboJsonHelper::getConfigFilePath(const QDir& dir, FileConflictResolutionMode::Enum conflictResolutionMode) {
        const QString configName("pbo");
        const QString configExt(".json");

        QString path = dir.absoluteFilePath(configName + configExt);
        const FileConflictResolutionPolicy policy(conflictResolutionMode);
        path = policy.resolvePotentialConflicts(path);

        return path;
    }

    QString PboJsonHelper::allocateTempConfigFilePath() {
        return QString();
    }

    void PboJsonHelper::saveTo(const PboJson& options, const QString& fileName) {
        const QJsonObject json = options.makeJson();
        const QByteArray bytes = QJsonDocument(json).toJson(QJsonDocument::Indented);

        QFile file(fileName);
        if (!file.open(QIODeviceBase::WriteOnly)) {
            throw DiskAccessException("Can not access the file. Check if it is used by other processes.", fileName);
        }
        file.write(bytes);
        file.close();
    }

    void PboJsonHelper::extractHeaders(const PboDocument& document, PboJson& options) {
        for (const DocumentHeader* header : *document.headers()) {
            options.headers.append(PboJsonHeader(header->name(), header->value()));
        }
    }

    constexpr const char* extensions[] = {"txt", "xml", "csv"};
    constexpr const char* files[] = {"mission.sqm", "description.ext"};

    void PboJsonHelper::extractCompressionRules(const PboDocument& document, PboJson& options) {
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
            if (const PboNode* node = document.root()->get(PboPath{file}); node) {
                if (IsCompressed(node->binarySource)) {
                    const QString rule = makeFileCompressionRule(file);
                    options.compress.include.append(rule);
                }
            }
        }
    }

    void PboJsonHelper::collectValuableArtifacts(const PboNode* node, QSet<QString>& results) {
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

    QString PboJsonHelper::makeExtensionCompressionRule(const QString& ext) {
        return "\\." + ext + "$";
    }

    QString PboJsonHelper::makeFileCompressionRule(const QString& fileName) {
        return "^" + fileName + "$";
    }
}
