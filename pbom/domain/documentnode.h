#pragma once

#include <QObject>
#include "pbonodetype.h"
#include "pbopath.h"
#include "conflictresolution.h"
#include "binarysource.h"
#include "abstractnode.h"

namespace pboman3::domain {
    class DocumentNode final : public AbstractNode<DocumentNode> {
    Q_OBJECT

    public:
        QSharedPointer<BinarySource> binarySource;

        DocumentNode(QString title, PboNodeType nodeType, DocumentNode* parentNode);

        DocumentNode* createHierarchy(const PboPath& entryPath);

        DocumentNode* createHierarchy(const PboPath& entryPath, const ConflictResolution& onConflict);

        void removeFromHierarchy();

        bool isPathConflict(const PboPath& path) const;

        const QString& title() const;

        void setTitle(QString title);

        PboNodeType nodeType() const;

        DocumentNode* get(const PboPath& path);

        PboPath makePath() const;

        bool operator <(const DocumentNode& node) const;

        friend QDebug operator <<(QDebug debug, const DocumentNode& node);

    signals:
        void titleChanged(const QString& title);

        void childCreated(DocumentNode* child, qsizetype index);

        void childMoved(qsizetype prevIndex, qsizetype newIndex);

        void childRemoved(qsizetype index);

        void hierarchyChanged();

    private:
        PboNodeType nodeType_;
        QString title_;

        DocumentNode* createHierarchy(const PboPath& entryPath, const ConflictResolution& onConflict, bool emitEvents);

        DocumentNode* findChild(const QString& title) const;

        QString pickFolderTitle(const DocumentNode* parent, const QString& expectedTitle) const;

        QString pickFileTitle(const DocumentNode* parent, const QString& expectedTitle) const;

        QString formatFolderTitleCopy(const QString& expectedTitle, qsizetype copyIndex) const;

        QString formatFileTitleCopy(const QString& expectedTitle, qsizetype copyIndex) const;

        DocumentNode* createChild(const QString& title, PboNodeType nodeType);

        qsizetype getChildListIndex(const DocumentNode* node) const;

        void emitHierarchyChanged();
    };
}
