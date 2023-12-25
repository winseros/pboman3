#include "execbackend.h"
#include <QDir>
#include <QFileInfo>
#include <QUuid>
#include "io/diskaccessexception.h"

namespace pboman3::io {
    ExecBackend::ExecBackend(const QDir& folder)
        : folder_(folder) {
        if (!folder.exists())
            throw InvalidOperationException("The folder provided must exist");
        nodeFileSystem_ = QSharedPointer<NodeFileSystem>(new NodeFileSystem(folder));
    }

    ExecBackend::~ExecBackend() {
        folder_.removeRecursively();
    }

    QString ExecBackend::execSync(const PboNode* node, const Cancel& cancel) {
        assert(node->nodeType() == PboNodeType::File && "Can exec only file nodes");

        QString result;

        const QString path = nodeFileSystem_->composeRelativePath(node);

        if (tryFindExisting(path, &result))
            return result;

        result = extractNew(path, node, cancel);

        return result;
    }

    void ExecBackend::clear(const PboNode* node) {
        assert(node->nodeType() == PboNodeType::File && "Can clear only file nodes");

        const QString path = nodeFileSystem_->composeRelativePath(node);
        if (extracted_.contains(path))
            extracted_.remove(path);
    }

    bool ExecBackend::tryFindExisting(const QString& path, QString* result) {
        if (extracted_.contains(path)) {
            QList<CacheData>& cache = extracted_[path];
            auto it = cache.begin();
            while (it != cache.end()) {
                const ReuseResult reuse = tryReuse(*it, result);
                switch (reuse) {
                    case ReuseResult::CanReuse:
                        return true;
                    case ReuseResult::CantReuseTemp:
                        ++it;
                        break;
                    case ReuseResult::CantReusePerm:
                        cache.erase(it);
                        break;
                }
            }
            if (cache.empty())
                extracted_.remove(path);
        }
        return false;
    }

    ExecBackend::ReuseResult ExecBackend::tryReuse(const CacheData& cacheItem, QString* result) const {
        ReuseResult status = ReuseResult::CantReuseTemp;

        const QFileInfo fi(cacheItem.path);
        if (fi.exists() && fi.lastModified() == cacheItem.lastModified) {
            if (QFile(cacheItem.path).open(QIODeviceBase::ReadWrite)) {
                status = ReuseResult::CanReuse;
                *result = cacheItem.path;
            }
        } else {
            status = ReuseResult::CantReusePerm;
        }

        return status;
    }

    QString ExecBackend::extractNew(const QString& path, const PboNode* node, const Cancel& cancel) {
        const QString key = QUuid::createUuid().toString(QUuid::WithoutBraces);

        QString execPath = folder_.filePath(key + QDir::separator() + path);
        const QFileInfo fi(execPath);

        if (!folder_.mkpath(folder_.relativeFilePath(fi.dir().path())))
            throw DiskAccessException("Could not create the folder.", fi.dir().path());

        QFile file(execPath);
        if (!file.open(QIODeviceBase::ReadWrite | QIODeviceBase::NewOnly))
            throw DiskAccessException(
                "Could not open file. Check you have enough permissions and the file is not locked by another process.",
                execPath);

        const auto bsClose = qScopeGuard([&node] { if (node->binarySource->isOpen()) node->binarySource->close(); });
        node->binarySource->open();
        node->binarySource->writeToFs(&file, cancel);

        file.close();

        if (cancel())
            file.remove();

        CacheData cacheData;
        cacheData.path = execPath;
        cacheData.lastModified = fi.lastModified();
        extracted_[path].append(cacheData);

        return execPath;
    }
}
