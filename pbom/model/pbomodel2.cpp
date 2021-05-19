#include "pbomodel2.h"
#include <QDir>
#include <QUrl>
#include "pbotreeexception.h"
#include "io/pboioexception.h"
#include "io/pboheaderreader.h"
#include "io/pbowriter.h"
#include "io/bs/pbobinarysource.h"

namespace pboman3 {
    void PboModel2::loadFile(const QString& path) {
        if (file_) {
            file_->close();
        }

        file_ = QSharedPointer<PboFile>(new PboFile(path));
        file_->open(QIODeviceBase::OpenModeFlag::ReadWrite);

        root_ = QSharedPointer<PboNode>(new PboNode("container", PboNodeType::Container, nullptr, nullptr));
        connect(root_.get(), &PboNode::onEvent, this, &PboModel2::onEvent);

        emitLoadBegin(path);

        try {
            PboFileHeader header = PboHeaderReader::readFileHeader(file_.get());

            size_t entryDataOffset = file_->pos();
            for (const QSharedPointer<PboEntry>& entry : header.entries) {
                PboNode* node = root_->addEntry(entry->makePath());
                PboDataInfo dataInfo{entry->originalSize(), entry->dataSize(), entryDataOffset};
                entryDataOffset += dataInfo.dataSize;
                node->binarySource = QSharedPointer<PboBinarySource>(
                    new PboBinarySource(path, dataInfo));
            }

            binaryBackend_ = QSharedPointer<BinaryBackend>(new BinaryBackend);
            headers_ = header.headers;

            emitLoadComplete(path);
        } catch (PboIoException&) {
            emitLoadFailed();
        }
    }

    void PboModel2::saveFile(const Cancel& cancel) {
        PboWriter writer;
        writer.usePath(file_->fileName() + ".t")
              .useRoot(root_.get());

        for (const QSharedPointer<PboHeader>& header : headers_) {
            writer.addHeader(header.get());
        }

        writer.write(cancel);
    }

    void PboModel2::createNodeSet(const PboPath& parent, const QList<NodeDescriptor>& descriptors,
                                  const ConflictsParcel& conflicts) const {
        if (!root_)
            throw PboTreeException("The model is not initialized");
        PboNode* node = root_->get(parent);
        if (!node)
            throw PboTreeException("The requested parent does not exist");

        for (const NodeDescriptor& descriptor : descriptors) {
            const ConflictResolution resolution = conflicts.getResolution(descriptor);
            if (resolution != ConflictResolution::Skip) {
                PboNode* created = node->addEntry(PboPath(descriptor.path()), resolution);
                created->binarySource = descriptor.binarySource();
            }
        }
    }

    void PboModel2::renameNode(const PboPath& node, const QString& title) const {
        if (!root_)
            throw PboTreeException("The model is not initialized");
        root_->renameNode(node, title);
    }

    void PboModel2::removeNode(const PboPath& node) const {
        if (!root_)
            throw PboTreeException("The model is not initialized");
        root_->removeNode(node);
    }

    InteractionParcel PboModel2::interactionPrepare(const QList<PboPath>& paths, const Cancel& cancel) const {
        QList<PboNode*> sync;
        sync.reserve(paths.length());

        for (const PboPath& p : paths) {
            PboNode* node = root_->get(p);
            sync.append(node);
        }

        QList<QUrl> files = binaryBackend_->hddSync(sync, cancel);
        NodeDescriptors nodes = NodeDescriptors::packTree(root_.get(), paths);
        return InteractionParcel(std::move(files), std::move(nodes));
    }

    ConflictsParcel PboModel2::checkConflicts(const PboPath& parent, const QList<NodeDescriptor>& descriptors) const {
        if (!root_)
            throw PboTreeException("The model is not initialized");
        PboNode* node = root_->get(parent);
        if (!node)
            throw PboTreeException("The requested parent does not exist");

        ConflictsParcel conflicts;
        for (const NodeDescriptor& descriptor : descriptors) {
            if (node->get(PboPath(descriptor.path()))) {
                conflicts.setResolution(descriptor, ConflictResolution::Copy);
            }
        }
        return conflicts;
    }

    void PboModel2::registerHeader(QSharedPointer<PboHeader>& header) {
        const PboHeaderCreatedEvent evt(header.get());
        emit onEvent(&evt);
        headers_.append(header);
    }

    void PboModel2::emitLoadBegin(const QString& path) const {
        const PboLoadBeginEvent evt(path);
        emit onEvent(&evt);
    }

    void PboModel2::emitLoadComplete(const QString& path) const {
        PboLoadCompleteEvent evt(path);
        emit onEvent(&evt);
    }

    void PboModel2::emitLoadFailed() const {
        PboLoadFailedEvent evt;
        emit onEvent(&evt);
    }
}
