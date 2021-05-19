#pragma once

#include <QObject>
#include "pbonodeevents.h"
#include "pbonodetype.h"
#include "pbopath.h"
#include "conflictresolution.h"
#include "io/bs/binarysource.h"

namespace pboman3 {
    class PboNode final : public QObject {
    Q_OBJECT

    public:
        QSharedPointer<BinarySource> binarySource;

        PboNode(QString title, PboNodeType nodeType, PboNode* par, PboNode* root);

        ~PboNode() override;

        PboNode* addEntry(const PboPath& entryPath);

        PboNode* addEntry(const PboPath& entryPath, const ConflictResolution& onConflict);

        PboPath makePath() const;

        PboNodeType nodeType() const;

        const QString& title() const;

        PboNode* par() const;

        PboNode* root() const;

        bool fileExists(const PboPath& path);

        PboNode* get(const PboPath& path);

        PboNode* child(int index) const;

        int childCount() const;

        QList<PboNode*>::iterator begin();

        QList<PboNode*>::iterator end();

        QList<PboNode*>::const_iterator begin() const;

        QList<PboNode*>::const_iterator end() const;

        void renameNode(const PboPath& node, const QString& title);

        void removeNode(const PboPath& node);

    signals:
        void onEvent(const PboNodeEvent* evt) const;

    private:
        PboNodeType nodeType_;
        QString title_;
        PboNode* par_;
        PboNode* root_;
        QList<PboNode*> children_;

        PboNode* createFolderHierarchy(const PboPath& path);

        PboNode* createFileNode(const QString& title, PboNode* parent) const;

        PboNode* findChild(const QString& title) const;

        QString resolveNameConflict(const PboNode* parent, const PboNode* node) const;
    };
}
