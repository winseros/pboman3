#pragma once

#include <QObject>
#include "pbonodetype.h"
#include "pbopath.h"
#include "conflictresolution.h"
#include "binarysource.h"
#include "abstractnode.h"

namespace pboman3::domain {
    class PboNodeTransaction;

    class PboNode final : public AbstractNode<PboNode>, public QEnableSharedFromThis<PboNode> {
    Q_OBJECT

    public:
        QSharedPointer<BinarySource> binarySource;

        PboNode(QString title, PboNodeType nodeType, PboNode* parentNode);

        PboNode* createHierarchy(const PboPath& entryPath);

        PboNode* createHierarchy(const PboPath& entryPath, const ConflictResolution& onConflict);

        void removeFromHierarchy();

        const QString& title() const;

        PboNodeType nodeType() const;

        PboNode* get(const PboPath& path);

        const PboNode* get(const PboPath& path) const;

        PboPath makePath() const;

        QSharedPointer<PboNodeTransaction> beginTransaction();

        bool operator <(const PboNode& node) const;

        friend QDebug operator <<(QDebug debug, const PboNode& node);

    signals:
        void titleChanged(const QString& title);

        void childCreated(PboNode* child, qsizetype index);

        void childMoved(qsizetype prevIndex, qsizetype newIndex);

        void childRemoved(qsizetype index);

        void hierarchyChanged();

    private:
        PboNodeType nodeType_;
        QString title_;

        PboNode* createHierarchy(const PboPath& entryPath, const ConflictResolution& onConflict, bool emitEvents);

        QString pickFolderTitle(const QString& expectedTitle) const;

        QString pickFileTitle(const QString& expectedTitle) const;

        PboNode* createNode(const QString& title, PboNodeType nodeType);

        void removeNode(const QSharedPointer<PboNode>& node);

        void emitHierarchyChanged();

        void setTitle(QString title);

    friend PboNodeTransaction;
    };
}
