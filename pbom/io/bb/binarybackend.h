#pragma once

#include "execbackend.h"
#include "tempbackend.h"
#include "domain/pbonode.h"
#include <QDir>

namespace pboman3::io {
    using namespace domain;

    class BinaryBackend {
    public:
        BinaryBackend(const QString& name);

        QList<QUrl> hddSync(const QList<PboNode*>& nodes, const Cancel& cancel) const;

        QString execSync(const PboNode* node, const Cancel& cancel) const;

        void unpackSync(const QDir& dest, const PboNode* rootNode, const QList<PboNode*>& childNodes,
                        const Cancel& cancel) const;

        void clear(const PboNode* node) const;

        QDir getTempDir() const;

    private:
        QSharedPointer<TempBackend> tempBackend_;
        QSharedPointer<ExecBackend> execBackend_;
        QString tempPath_;
    };
}
