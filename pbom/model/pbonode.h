#pragma once

#include <QPointer>
#include <qobject.h>
#include "pbonodeevents.h"
#include "pbonodetype.h"
#include "pbopath.h"
#include "model/pboentry.h"

namespace pboman3 {
    enum class PboConflictResolution {
        Abort,
        Copy,
        ReplaceOrMerge
    };

    class PboNode final : public QObject {
    Q_OBJECT

    public:
        PboNode(QString title, PboNodeType nodeType, const QPointer<PboNode>& par, const QPointer<PboNode>& root);

        ~PboNode() override;

        void addEntry(const PboEntry* entry);

        PboPath makePath() const;

        PboNodeType nodeType() const;

        const QString& title() const;

        const QPointer<PboNode>& par() const;

        const QPointer<PboNode>& root() const;

        QPointer<PboNode> get(const PboPath& node) const;

        QPointer<PboNode> child(int index) const;

        int childCount() const;

        void addNode(const PboPath& node);

        void moveNode(const PboPath& node, const PboPath& newParent,
                      PboConflictResolution (*onConflict)(const PboPath&, PboNodeType));

        void renameNode(const PboPath& node, const QString& title);

        void removeNode(const PboPath& node);

    signals:
        void onEvent(const PboNodeEvent* evt) const;

    private:
        PboNodeType nodeType_;
        QString title_;
        QPointer<PboNode> par_;
        QPointer<PboNode> root_;
        QList<QPointer<PboNode>> children_;

        QPointer<PboNode> findChild(const QString& title) const;

        QString resolveNameConflict(const QPointer<PboNode>& parent, const QPointer<PboNode>& node) const;

        void moveNodeImpl(QPointer<PboNode>& node, QPointer<PboNode>& newParent,
                          PboConflictResolution (*onConflict)(
                              const PboPath&, PboNodeType));

        void cleanupNodeHierarchy(QPointer<PboNode>& node);

        bool tryCleanupNode(QPointer<PboNode>& node);
    };
}
