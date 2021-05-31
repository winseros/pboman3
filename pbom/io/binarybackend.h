#pragma once

#include "model/pbonode.h"
#include "util/util.h"

namespace pboman3 {
    class BinaryBackend {
    public:
        BinaryBackend(const QString& name);

        ~BinaryBackend();

        QList<QUrl> hddSync(const QList<PboNode*>& nodes, const Cancel& cancel) const;

    private:
        QString tree_;

        QString syncPboFileNode(const PboNode* node, const Cancel& cancel) const;

        QString syncPboDirNode(const PboNode* node, const Cancel& cancel) const;

        void syncPboDir(const PboNode* node, const Cancel& cancel) const;

        QString makeFsPath(const PboPath& pboPath) const;
    };
}
