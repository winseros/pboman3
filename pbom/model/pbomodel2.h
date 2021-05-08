#pragma once

#include <QFuture>
#include <QObject>
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

        void saveFile();

        // void createNodeSet(const PboPath& parent, const FilesystemFiles& data, const ResolveConflictsFn& onConflict) const;
        //
        // void createNodeSet(const PboPath& parent, const QByteArray& data, const ResolveConflictsFn& onConflict) const;

        void createNodeSet(const PboPath& parent, const QList<NodeDescriptor>& descriptors) const;

        void renameNode(const PboPath& node, const QString& title) const;

        void removeNode(const PboPath& node) const;

        InteractionParcel interactionPrepare(const QList<PboPath>& paths, const Cancel& cancel) const;

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

        void writeFileSignature(const PboHeaderIO& io) const;
    };
}
