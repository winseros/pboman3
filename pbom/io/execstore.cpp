#include "execstore.h"
#include <QDir>
#include <QUuid>
#include "pboioexception.h"

namespace pboman3 {
    ExecStore::ExecStore(QString fileSystemPath)
        : fileSystemPath_(std::move(fileSystemPath)) {
    }

    QString ExecStore::execSync(const PboNode* node, const Cancel& cancel) {
        if (node->nodeType() != PboNodeType::File)
            throw PboIoException("Can exec only file nodes");

        QString result;

        const QString path = getFileSystemPath(node->makePath());

        if (tryFindExisting(path, &result))
            return result;

        result = extractNew(path, node, cancel);

        return result;
    }

    bool ExecStore::tryFindExisting(const QString& path, QString* result) {
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

    QString ExecStore::getFileSystemPath(const PboPath& path) const {
        qsizetype expectedLength = 0;

        for (const QString& segment : path)
            expectedLength = expectedLength + segment.length() + 1; //add 1 separator byte for each segment

        QString result = "";
        result.reserve(expectedLength);

        for (const QString& segment : path)
            result.append(QDir::separator()).append(segment);

        return result;
    }

    ExecStore::ReuseResult ExecStore::tryReuse(const CacheData& cacheItem, QString* result) const {
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

    QString ExecStore::extractNew(const QString& path, const PboNode* node, const Cancel& cancel) {
        const QString key = QUuid::createUuid().toString(QUuid::WithoutBraces);

        const QFileInfo fi(fileSystemPath_ + QDir::separator() + key + path);
        QString execPath = fi.absoluteFilePath();

        const QString tempDirPath = QDir::temp().relativeFilePath(fi.dir().absolutePath());
        if (!QDir::temp().mkpath(tempDirPath))
            throw PboIoException("Could not create folder in temp: " + tempDirPath);

        QFile file(execPath);
        if (!file.open(QIODeviceBase::ReadWrite | QIODeviceBase::NewOnly))
            throw PboIoException("Could not open file: " + execPath);

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
