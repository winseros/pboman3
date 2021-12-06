#include "packconfiguration.h"
#include <QFile>
#include <QJsonDocument>
#include <QRegularExpression>
#include "domain/documentheaderstransaction.h"
#include "domain/func.h"
#include "io/diskaccessexception.h"
#include "model/binarysourceutils.h"

namespace pboman3::model::task {
    PackConfiguration::PackConfiguration(PboDocument* document)
        : document_(document) {
    }

    void PackConfiguration::apply() const {
        PboNode* pboJson = FindDirectChild(document_->root(), "pbo.json");
        if (pboJson) {
            const PackOptions packOptions = readPackOptions(pboJson);
            applyDocumentHeaders(packOptions);
            const CompressionRules compressionRules = buildCompressionRules(packOptions);
            applyDocumentCompressionRules(document_->root(), compressionRules);
        }
        if (!pboJson) {
            applyFileContentAsHeader("prefix");
            applyFileContentAsHeader("product");
            applyFileContentAsHeader("version");
        }
        if (pboJson)
            pboJson->removeFromHierarchy();
    }

    void PackConfiguration::applyDocumentHeaders(const PackOptions& options) const {
        if (options.headers().isEmpty())
            return;

        const QSharedPointer<DocumentHeadersTransaction> tran = document_->headers()->beginTransaction();
        for (const PackHeader& header : options.headers()) {
            tran->add(header.name(), header.value());
        }

        tran->commit();
    }

    void PackConfiguration::applyDocumentCompressionRules(PboNode* node, const CompressionRules& rules) const {
        if (node->nodeType() == PboNodeType::File) {
            if (shouldCompress(node, rules)) {
                ChangeBinarySourceCompressionMode(node->binarySource, true);
            }
        } else {
            for (PboNode* child : *node) {
                applyDocumentCompressionRules(child, rules);
            }
        }
    }

    bool PackConfiguration::shouldCompress(const PboNode* node, const CompressionRules& rules) {
        const QString path = node->makePath().toString();

        bool include = false;
        for (const QRegularExpression& rule : rules.include) {
            if (rule.match(path).hasMatch()) {
                include = true;
                break;
            }
        }
        if (include) {
            for (const QRegularExpression& rule : rules.exclude) {
                if (rule.match(path).hasMatch()) {
                    include = false;
                    break;
                }
            }
        }
        return include;
    }

    PackConfiguration::CompressionRules PackConfiguration::buildCompressionRules(const PackOptions& options) {
        CompressionRules rules;
        convertToCompressionRules(options.compress().include(), &rules.include);
        convertToCompressionRules(options.compress().exclude(), &rules.exclude);
        return rules;
    }

    void PackConfiguration::convertToCompressionRules(const QList<QString>& source, QList<QRegularExpression>* dest) {
        dest->reserve(source.count());

        for (const QString& rule : source) {
            QRegularExpression reg(
                rule, QRegularExpression::CaseInsensitiveOption | QRegularExpression::DontCaptureOption);
            if (!reg.isValid()) {
                throw JsonStructureException(
                    "The regular expression \"" + reg.pattern() + "\" is invalid: " + reg.errorString());
            }
            dest->append(std::move(reg));
        }
    }

    PackOptions PackConfiguration::readPackOptions(const PboNode* node) {
        const QByteArray data = readNodeContent(node);
        QJsonParseError err;
        const QJsonDocument json = QJsonDocument::fromJson(data, &err);
        if (json.isNull()) {
            throw JsonStructureException(err.errorString() + " at offset " + QString::number(err.offset));
        }
        if (!json.isObject()) {
            throw JsonStructureException("The json must contain an object");
        }

        PackOptions opts;
        opts.settle(json.object(), "");
        return opts;
    }

    QByteArray PackConfiguration::readNodeContent(const PboNode* node) {
        QFile f(node->binarySource->path());
        if (!f.open(QIODeviceBase::ReadOnly)) {
            throw io::DiskAccessException("Could not read the file", f.fileName());
        }
        QByteArray data = f.readAll();

        return data;
    }

    void PackConfiguration::applyFileContentAsHeader(const QString& prefix) const {
        PboNode* node = FindDirectChild(document_->root(), "$" + prefix + "$");
        if (node) {
            const QByteArray data = readNodeContent(node);
            const QSharedPointer<DocumentHeadersTransaction> tran = document_->headers()->beginTransaction();
            tran->add(prefix, data);
            tran->commit();
            node->removeFromHierarchy();
        }
    }
}
