#pragma once

#include <QFuture>
#include <QObject>
#include "conflictsparcel.h"
#include "interactionparcel.h"
#include "pbonode.h"
#include "io/binarybackend.h"
#include "io/pboheaderio.h"

namespace pboman3 {
    class PboModel : public QObject {
    Q_OBJECT
    public:
        void loadFile(const QString& path);

        void saveFile(const Cancel& cancel, const QString& filePath = nullptr);

        void unloadFile();

        void createNodeSet(PboNode* parent, const QList<NodeDescriptor>& descriptors, const ConflictsParcel& conflicts) const;

        InteractionParcel interactionPrepare(const QList<PboNode*>& nodes, const Cancel& cancel) const;

        ConflictsParcel checkConflicts(PboNode* parent, const QList<NodeDescriptor>& descriptors) const;

        PboNode* rootEntry() const;

        const QString& loadedPath() const;

    signals:
        void modelChanged();

        void loadedPathChanged();

    private:
        QString loadedPath_;
        QSharedPointer<PboNode> rootEntry_;
        QSharedPointer<BinaryBackend> binaryBackend_;
        QList<QSharedPointer<PboHeader>> headers_;

        void setLoadedPath(const QString& loadedFile);

        void rootTitleChanged();
    };
}
