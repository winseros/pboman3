#include "packconfiguration.h"
#include <QFile>
#include <QJsonDocument>
#include <QRegularExpression>
#include "domain/documentheaderstransaction.h"
#include "domain/func.h"
#include "io/diskaccessexception.h"
#include "model/binarysourceutils.h"
#include "util/log.h"

#define LOG(...) LOGGER("model/task/PackConfiguration", __VA_ARGS__)

namespace pboman3::model::task {
    PrefixEncodingException::PrefixEncodingException(QString prefixFileName)
        : AppException(std::move(prefixFileName)) {
    }

    void PrefixEncodingException::raise() const {
        throw *this;
    }

    QException* PrefixEncodingException::clone() const {
        return new PrefixEncodingException(*this);
    }

    QDebug& operator<<(QDebug& debug, const PrefixEncodingException& ex) {
        return debug << "PrefixEncodingException(Message=" << ex.message_ << ")";
    }


    PackConfiguration::PackConfiguration(PboDocument* document)
        : document_(document) {
    }

    void PackConfiguration::apply() const {
        bool usedPboJson = false;
        PboNode* pboJson = FindDirectChild(document_->root(), "pbo.json");
        if (pboJson) {
            LOG(info, "Apply configuration from pbo.json")
            const PackOptions packOptions = readPackOptions(pboJson);
            applyDocumentHeaders(packOptions);
            const CompressionRules compressionRules = buildCompressionRules(packOptions);
            applyDocumentCompressionRules(document_->root(), compressionRules);
            pboJson->removeFromHierarchy();
            usedPboJson = true;
        }

        processPrefixFile("prefix", "$pboprefix$", "pboprefix.txt", usedPboJson);
        processPrefixFile("product", "$pboproduct$", "pboproduct.txt", usedPboJson);
        processPrefixFile("version", "$pboversion$", "pboversion.txt", usedPboJson);
    }

    void PackConfiguration::applyDocumentHeaders(const PackOptions& options) const {
        if (options.headers.isEmpty()) {
            LOG(info, "No headers defined in the config")
            return;
        }

        LOG(info, options.headers.count(), "headers defined in the config")

        const QSharedPointer<DocumentHeadersTransaction> tran = document_->headers()->beginTransaction();
        for (const PackHeader& header : options.headers) {
            LOG(debug, "Header: ", header.name, "|", header.value)
            tran->add(header.name, header.value);
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
        LOG(info, "Building include rules")
        convertToCompressionRules(options.compress.include, &rules.include);
        LOG(info, "Building exclude rules")
        convertToCompressionRules(options.compress.exclude, &rules.exclude);
        return rules;
    }

    void PackConfiguration::convertToCompressionRules(const QList<QString>& source, QList<QRegularExpression>* dest) {
        dest->reserve(source.count());

        for (const QString& rule : source) {
            QRegularExpression reg(
                rule, QRegularExpression::CaseInsensitiveOption | QRegularExpression::DontCaptureOption);
            if (!reg.isValid()) {
                LOG(warning, "Compression rule is invalid - throwing:", rule)
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
            LOG(warning, "Json was null when read - throwing")
            throw JsonStructureException(err.errorString() + " at offset " + QString::number(err.offset));
        }
        if (!json.isObject()) {
            LOG(warning, "Json root was not an object - throwing:", json)
            throw JsonStructureException("The json must contain an object");
        }

        PackOptions opts;
        opts.settle(json.object(), "");
        return opts;
    }

    QByteArray PackConfiguration::readNodeContent(const PboNode* node) {
        QFile f(node->binarySource->path());
        if (!f.open(QIODeviceBase::ReadOnly)) {
            LOG(warning, "Could not open the file - throwing:", node->binarySource->path())
            throw io::DiskAccessException("Could not read the file", f.fileName());
        }
        QByteArray data = f.readAll();

        return data;
    }

    void PackConfiguration::processPrefixFile(const QString& header, const QString& fileName,
                                              const QString& altFileName, bool cleanupOnly) const {
        PboNode* file1 = FindDirectChild(document_->root(), fileName);
        PboNode* file2 = FindDirectChild(document_->root(), altFileName);

        if (!cleanupOnly) {
            if (file2) {
                applyNodeContentAsHeader(file2, header);
            } else if (file1) {
                applyNodeContentAsHeader(file1, header);
            }
        }

        if (file1)
            file1->removeFromHierarchy();
        if (file2)
            file2->removeFromHierarchy();
    }

    void PackConfiguration::applyNodeContentAsHeader(const PboNode* node, const QString& header) const {
        LOG(info, "Apply prefix:", header)
        const QByteArray data = readNodeContent(node);
        throwIfBreaksPbo(node, data);
        const QSharedPointer<DocumentHeadersTransaction> tran = document_->headers()->beginTransaction();
        tran->add(header, data);
        tran->commit();
    }

    void PackConfiguration::throwIfBreaksPbo(const PboNode* node, const QByteArray& data) {
        for (const char& byte : data) {
            if (byte == 0) {
                LOG(warning, "The prefix file is invalid - throwing:", node->title())
                throw PrefixEncodingException(node->title());
            }
        }
    }
}
