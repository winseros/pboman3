#pragma once

#include <QObject>
#include "pbonodetype.h"
#include "pbopath.h"
#include "conflictresolution.h"
#include "io/bs/binarysource.h"
#include "util/qpointerlistiterator.h"

namespace pboman3 {
    typedef QString TitleError;

    class PboNode final : public QObject {
    Q_OBJECT

    public:
        QSharedPointer<BinarySource> binarySource;

        PboNode(QString title, PboNodeType nodeType, PboNode* parentNode);

        PboNode* createHierarchy(const PboPath& entryPath);

        PboNode* createHierarchy(const PboPath& entryPath, const ConflictResolution& onConflict);

        void removeFromHierarchy();

        const QString& title() const;

        void setTitle(QString title);

        TitleError verifyTitle(const QString& title) const;

        PboNodeType nodeType() const;

        PboNode* parentNode() const;

        PboNode* get(const PboPath& path);

        PboNode* at(qsizetype index) const;

        int depth() const;

        int count() const;

        PboPath makePath() const;

        QPointerListIterator<PboNode> begin();

        QPointerListIterator<PboNode> end();

        QPointerListConstIterator<PboNode> begin() const;

        QPointerListConstIterator<PboNode> end() const;

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
        PboNode* parentNode_;
        QList<QSharedPointer<PboNode>> children_;

        PboNode* createHierarchyFolders(const PboPath& path);

        PboNode* findChild(const QString& title, PboNodeType nodeType) const;

        PboNode* findChild(const QString& title) const;

        PboNode* createChild(const QString& title, PboNodeType nodeType);

        QString resolveNameConflict(const PboNode* parent, const PboNode* node) const;

        qsizetype getChildListIndex(const PboNode* node) const;

        void emitHierarchyChanged();
    };
}
