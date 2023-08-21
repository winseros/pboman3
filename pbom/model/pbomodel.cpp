#include "pbomodel.h"
#include "domain/pbonode.h"
#include "domain/func.h"
#include <QDir>
#include <QUrl>
#include "io/documentwriter.h"
#include "io/defaultdocumentreaderfactory.h"
#include "task/pbojsonhelper.h"
#include "exception.h"
#include "util/log.h"

#define LOG(...) LOGGER("model/PboModel", __VA_ARGS__)

namespace pboman3::model {
    using namespace io;

    PboModel::PboModel(QSharedPointer<ApplicationSettingsFacility> settingsFacility)
        : settingsFacility_(std::move(settingsFacility)) {
    }

    void PboModel::loadFile(const QString& path) {
        LOG(info, "Loading the file:", path)

        if (!loadedPath_.isNull()) {
            LOG(info, "Unloading the previous file")
            unloadFile();
        }

        setLoadedPath(path);

        const DocumentReader reader = DefaultDocumentReaderFactory::createDocumentReader(path);
        try {
            document_ = reader.read();
            LOG(info, "Read the document:", *document_)
        } catch (const AppException& ex) {
            LOG(info, "Could not load the file:", ex)
            setLoadedPath(nullptr);
            throw;
        }

        connect(document_.get(), &PboDocument::changed, this, &PboModel::modelChanged);
        connect(document_.get(), &PboDocument::titleChanged, this, &PboModel::titleChanged);

        LOG(info, "Creating the binary backend")
        binaryBackend_ = QSharedPointer<BinaryBackend>(
            new BinaryBackend(QUuid::createUuid().toString(QUuid::WithoutBraces)));

        emit loadedStatusChanged(true);
    }

    void PboModel::saveFile(const Cancel& cancel, const QString& filePath) {
        LOG(info, "Saving the model to:", filePath)

        const QString savePath = filePath.isNull() ? loadedPath_ : filePath;
        LOG(info, "The savePath was set as:", savePath)

        DocumentWriter writer(savePath);
        writer.write(document_.get(), cancel);

        if (!cancel()) {
            LOG(info, "Write process complete")
            setLoadedPath(savePath);
        } else {
            LOG(info, "Write process cancelled")
        }
    }

    void PboModel::unloadFile() {
        if (!document_)
            throw InvalidOperationException("The model is not initialized");

        LOG(info, "Unloading the current file")

        setLoadedPath(nullptr);

        emit loadedStatusChanged(false);

        document_.clear();
        binaryBackend_.clear();
    }

    void PboModel::createNodeSet(PboNode* parent, const QList<NodeDescriptor>& descriptors,
                                      const ConflictsParcel& conflicts) const {
        if (!document_)
            throw InvalidOperationException("The model is not initialized");

        LOG(info, "Creating the set of nodes, parent:", *parent)

        for (const NodeDescriptor& descriptor : descriptors) {
            LOG(debug, "Process the node descriptor:", descriptor)

            const ConflictResolution resolution = conflicts.getResolution(descriptor);
            LOG(debug, "The conflict resolution for the descriptor was set to:", static_cast<qint32>(resolution))

            if (resolution != ConflictResolution::Skip) {
                PboNode* created = parent->createHierarchy(descriptor.path(), resolution);
                created->binarySource = descriptor.binarySource();
                binaryBackend_->clear(created);
            }
        }
    }

    InteractionParcel PboModel::interactionPrepare(const QList<PboNode*>& nodes, const Cancel& cancel) const {
        LOG(info, "Preparing the interaction for", nodes.count(), "nodes")

        QList<QUrl> files = binaryBackend_->hddSync(nodes, cancel);
        LOG(info, "Got files:", files)

        NodeDescriptors descriptors = NodeDescriptors::packNodes(nodes);
        LOG(info, "Got descriptors:", descriptors)

        return {std::move(files), std::move(descriptors)};
    }

    QString PboModel::execPrepare(const PboNode* node, const Cancel& cancel) const {
        LOG(info, "Preparing the execution of the node", *node)

        QString file = binaryBackend_->execSync(node, cancel);
        LOG(info, "The node contents was stored to:", file)

        return file;
    }

    ConflictsParcel PboModel::checkConflicts(const PboNode* parent,
                                                  const QList<NodeDescriptor>& descriptors) const {
        if (!document_)
            throw InvalidOperationException("The model is not initialized");

        LOG(info, "Check conflicts for the set of descriptors")

        ConflictsParcel conflicts;
        for (const NodeDescriptor& descriptor : descriptors) {
            if (IsPathConflict(parent, PboPath(descriptor.path()))) {
                LOG(info, "The descriptor is in conflict:", descriptor)
                conflicts.setResolution(descriptor, ConflictResolution::Copy);
            }
        }

        return conflicts;
    }

    void PboModel::unpackNodesTo(const QDir& dest, const PboNode* rootNode,
                                      const QList<PboNode*>& childNodes,
                                      const Cancel& cancel) const {
        LOG(info, "Unpack", childNodes.count(), "nodes to", dest)
        binaryBackend_->unpackSync(dest, rootNode, childNodes, cancel);
    }

    PboDocument* PboModel::document() const {
        return document_.get();
    }

    const QString& PboModel::loadedPath() const {
        return loadedPath_;
    }

    bool PboModel::isLoaded() const {
        return !loadedPath_.isNull();
    }

    void PboModel::setLoadedPath(const QString& loadedFile) {
        if (loadedPath_ != loadedFile) {
            LOG(info, "Set the loadedPath to:", loadedFile)
            loadedPath_ = loadedFile;
            emit loadedPathChanged();
        }
    }

    void PboModel::titleChanged() {
        LOG(info, "The document title was changed")
        QFileInfo fi(loadedPath_);
        fi.setFile(fi.dir(), document_->root()->title());
        setLoadedPath(fi.absoluteFilePath());
    }
}
