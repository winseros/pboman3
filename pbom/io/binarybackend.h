#pragma once

#include "execstore.h"
#include "model/pbonode.h"
#include "util/util.h"
#include <QSharedPointer>

namespace pboman3 {
    class BinaryBackend {
    public:
        BinaryBackend(const QString& name);

        ~BinaryBackend();

        QList<QUrl> hddSync(const QList<PboNode*>& nodes, const Cancel& cancel) const;

        QString execSync(const PboNode* node, const Cancel& cancel);

    private:
        QString tree_;
        QString exec_;
        QSharedPointer<ExecStore> execStore_;

        QString syncPboFileNode(const PboNode* node, const Cancel& cancel) const;

        QString syncPboDirNode(const PboNode* node, const Cancel& cancel) const;

        void syncPboDir(const PboNode* node, const Cancel& cancel) const;

        QString makeFsPath(const PboPath& pboPath, const QString& location) const;
    };
}
