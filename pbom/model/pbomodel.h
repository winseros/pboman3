#pragma once

#include "domain/pbodocument.h"
#include <QFuture>
#include <QObject>
#include "conflictsparcel.h"
#include "interactionparcel.h"
#include "io/bb/binarybackend.h"

namespace pboman3::model {
    using namespace domain;

    class PboModel : public QObject {
        Q_OBJECT
    public:
        void loadFile(const QString& path);

        void saveFile(const Cancel& cancel, const QString& filePath = nullptr);

        void unloadFile();

        void createNodeSet(PboNode* parent, const QList<NodeDescriptor>& descriptors, const ConflictsParcel& conflicts) const;

        InteractionParcel interactionPrepare(const PboNode* rootNode, const QList<PboNode*>& nodes, const Cancel& cancel) const;

        QString execPrepare(const PboNode* node, const Cancel& cancel) const;

        ConflictsParcel checkConflicts(const PboNode* parent, const QList<NodeDescriptor>& descriptors) const;

        void unpackNodesTo(const QDir& dest, const PboNode* rootNode, const QList<PboNode*>& childNodes, const Cancel& cancel) const;

        void extractConfigurationTo(const QDir& dest) const;

        PboDocument* document() const;

        const QString& loadedPath() const;

        bool isLoaded() const;

    signals:
        void modelChanged();

        void loadedPathChanged();

        void loadedStatusChanged(bool loaded);

    private:
        QString loadedPath_;
        QSharedPointer<PboDocument> document_;
        QSharedPointer<BinaryBackend> binaryBackend_;

        void setLoadedPath(const QString& loadedFile);

        bool isExtractingContainer(const PboNode* rootNode, const QList<PboNode*>& childNodes) const;

        QUrl extractConfigurationToTempDir() const;

        void titleChanged();
    };
}
