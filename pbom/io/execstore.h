#pragma once

#include <QDateTime>
#include "domain/pbonode.h"
#include "util/util.h"

namespace pboman3 {
    class ExecStore {
    public:
        explicit ExecStore(QString fileSystemPath);

        QString execSync(const PboNode* node, const Cancel& cancel);

        void cleanStoredData(const PboNode* node);

    private:
        struct CacheData {
            QString path;
            QDateTime lastModified;
        };

        enum class ReuseResult {
            CanReuse,
            CantReuseTemp,
            CantReusePerm
        };

        QString fileSystemPath_;
        QHash<QString, QList<CacheData>> extracted_;

        bool tryFindExisting(const QString& path, QString* result);

        QString getFileSystemPath(const PboPath& path) const;

        ReuseResult tryReuse(const CacheData& cacheItem, QString* result) const;

        QString extractNew(const QString& path, const PboNode* node, const Cancel& cancel);
    };
}
