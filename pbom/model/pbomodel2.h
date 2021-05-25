#pragma once

#include <QFuture>
#include <QObject>
#include "conflictsparcel.h"
#include "interactionparcel.h"
#include "pbomodelevents.h"
#include "pbonode.h"
#include "io/binarybackend.h"
#include "io/pbofile.h"
#include "io/pboheaderio.h"

namespace pboman3 {
    class PboModel2 : public QObject {
    Q_OBJECT
    public:
        void loadFile(const QString& path);

        void saveFile(const Cancel& cancel);

        void unloadFile();

        void createNodeSet(const PboPath& parent, const QList<NodeDescriptor>& descriptors, const ConflictsParcel& conflicts) const;

        void renameNode(const PboPath& node, const QString& title) const;

        void removeNode(const PboPath& node) const;

        InteractionParcel interactionPrepare(const QList<PboPath>& paths, const Cancel& cancel) const;

        ConflictsParcel checkConflicts(const PboPath& parent, const QList<NodeDescriptor>& descriptors) const;

        bool doesExist(const PboPath& path) const;

    signals:
        void onEvent(const PboModelEvent* event) const;

    private:
        QSharedPointer<PboFile> file_;
        QSharedPointer<PboNode> root_;
        QSharedPointer<BinaryBackend> binaryBackend_;
        QList<QSharedPointer<PboHeader>> headers_;

        void registerHeader(QSharedPointer<PboHeader>& header);

        void emitLoadBegin(const QString& path) const;

        void emitLoadComplete(const QString& path) const;

        void emitLoadFailed() const;

        void emitUnload() const;
    };
}
