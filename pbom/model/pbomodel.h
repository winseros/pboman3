#pragma once

#include <QFuture>
#include <QObject>
#include "conflictsparcel.h"
#include "headersmodel.h"
#include "interactionparcel.h"
#include "pbonode.h"
#include "signaturemodel.h"
#include "io/binarybackend.h"

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

        HeadersModel* headers() const;

        SignatureModel* signature() const;

        const QString& loadedPath() const;

    signals:
        void modelChanged();

        void loadedPathChanged();

    private:
        QString loadedPath_;
        QSharedPointer<PboNode> rootEntry_;
        QSharedPointer<HeadersModel> headers_;
        QSharedPointer<SignatureModel> signature_;
        QSharedPointer<BinaryBackend> binaryBackend_;

        void setLoadedPath(const QString& loadedFile);

        void rootTitleChanged();
    };
}
