#pragma once

#include <QObject>
#include "pbonodetype.h"
#include "pbopath.h"
#include "conflictresolution.h"
#include "binarysource.h"
#include "abstractnode.h"

namespace pboman3::domain {
    class PboNodeTransaction;

    class PboNode final : public AbstractNode<PboNode> {
    Q_OBJECT

    public:
        QSharedPointer<BinarySource> binarySource;

        PboNode(QString title, PboNodeType nodeType, PboNode* parentNode);

        PboNode* createHierarchy(const PboPath& entryPath);

        PboNode* createHierarchy(const PboPath& entryPath, const ConflictResolution& onConflict);

        void removeFromHierarchy();

        bool isPathConflict(const PboPath& path) const;

        const QString& title() const;

        PboNodeType nodeType() const;

        PboNode* get(const PboPath& path);

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

        PboNode* findChild(const QString& title) const;

        QString pickFolderTitle(const PboNode* parent, const QString& expectedTitle) const;

        QString pickFileTitle(const PboNode* parent, const QString& expectedTitle) const;

        QString formatFolderTitleCopy(const QString& expectedTitle, qsizetype copyIndex) const;

        QString formatFileTitleCopy(const QString& expectedTitle, qsizetype copyIndex) const;

        PboNode* createChild(const QString& title, PboNodeType nodeType);

        qsizetype getChildListIndex(const PboNode* node) const;

        void emitHierarchyChanged();

        void setTitle(QString title);

        friend PboNodeTransaction;
    };
}
