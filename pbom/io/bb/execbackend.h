#pragma once

#include "nodefilesystem.h"
#include "domain/pbonode.h"

namespace pboman3 {
    class ExecBackend {
    public:
        explicit ExecBackend(const QDir& folder);

        ~ExecBackend();

        QString execSync(const PboNode* node, const Cancel& cancel);

        void clear(const PboNode* node);

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

        QDir folder_;
        QSharedPointer<NodeFileSystem> nodeFileSystem_;
        QHash<QString, QList<CacheData>> extracted_;

        bool tryFindExisting(const QString& path, QString* result);

        ReuseResult tryReuse(const CacheData& cacheItem, QString* result) const;

        QString extractNew(const QString& path, const PboNode* node, const Cancel& cancel);
    };
}
