#pragma once

#include <QObject.h>
#include <QPointer>
#include <functional>
#include "pbonodeevents.h"
#include "pbonodetype.h"
#include "pboparcel.h"
#include "pbopath.h"
#include "io/bs/binarysource.h"

namespace pboman3 {
 
    enum class PboConflictResolution {
        Abort,
        Copy,
        Replace
    };

    typedef std::function<PboConflictResolution(const PboPath&, PboNodeType)> OnConflict;

    class PboNode final : public QObject {
    Q_OBJECT

    public:
        QSharedPointer<BinarySource> binarySource;

        PboNode(QString title, PboNodeType nodeType, const QPointer<PboNode>& par, const QPointer<PboNode>& root);

        ~PboNode() override;

        void addEntry(const PboPath& entryPath);

        QPointer<PboNode> addEntry(const PboPath& entryPath, const OnConflict& onConflict);

        PboPath makePath() const;

        PboNodeType nodeType() const;

        const QString& title() const;

        const QPointer<PboNode>& par() const;

        const QPointer<PboNode>& root() const;

        QPointer<PboNode> get(const PboPath& node) const;

        QPointer<PboNode> child(int index) const;

        int childCount() const;

        QList<QPointer<PboNode>>::iterator begin();

        QList<QPointer<PboNode>>::iterator end();

        QList<QPointer<PboNode>>::const_iterator begin() const;

        QList<QPointer<PboNode>>::const_iterator end() const;

        void renameNode(const PboPath& node, const QString& title) const;

        void removeNode(const PboPath& node) const;

    signals:
        void onEvent(const PboNodeEvent* evt) const;

    private:
        PboNodeType nodeType_;
        QString title_;
        QPointer<PboNode> par_;
        QPointer<PboNode> root_;
        QList<QPointer<PboNode>> children_;

        QPointer<PboNode> createFolderHierarchy(const PboPath& path);

        QPointer<PboNode> createFileNode(const QString& title, QPointer<PboNode>& parent) const;

        QPointer<PboNode> findChild(const QString& title) const;

        QString resolveNameConflict(const QPointer<PboNode>& parent, const QPointer<PboNode>& node) const;
    };
}
