#pragma once

#include "nodefilesystem.h"
#include "domain/pbonode.h"
#include "util/util.h"

namespace pboman3::io {
    using namespace domain;

    class TempBackend {
    public:
        TempBackend(const QDir& folder);

        ~TempBackend();

        QList<QUrl> hddSync(const QList<PboNode*>& nodes, const Cancel& cancel) const;

        void clear(const PboNode* node) const;

    private:
        QDir folder_;
        QSharedPointer<NodeFileSystem> nodeFileSystem_;

        QString syncPboFileNode(const PboNode* node, const Cancel& cancel) const;

        QString syncPboDirNode(const PboNode* node, const Cancel& cancel) const;

        void syncPboDir(const PboNode* node, const Cancel& cancel) const;
    };
}
