#include "pbomodel2.h"
#include <QDir>
#include <QUrl>
#include "pbotreeexception.h"
#include "io/pboheaderreader.h"
#include "io/pbowriter.h"
#include "io/bs/pbobinarysource.h"

namespace pboman3 {
    void PboModel2::loadFile(const QString& path) {
        if (file_)
            unloadFile();

        file_ = QSharedPointer<PboFile>(new PboFile(path));
        file_->open(QIODeviceBase::OpenModeFlag::ReadWrite);

        QString title = QFileInfo(path).fileName();
        rootEntry_ = QSharedPointer<PboNode>(new PboNode(std::move(title), PboNodeType::Container, nullptr));
        connect(rootEntry_.get(), &PboNode::hierarchyChanged, this, &PboModel2::modelChanged);

        PboFileHeader header = PboHeaderReader::readFileHeader(file_.get());

        size_t entryDataOffset = file_->pos();
        for (const QSharedPointer<PboEntry>& entry : header.entries) {
            PboNode* node = rootEntry_->createHierarchy(entry->makePath());
            PboDataInfo dataInfo(
                entry->originalSize(),
                entry->dataSize(),
                entryDataOffset,
                entry->packingMethod() == PboPackingMethod::Packed);
            entryDataOffset += dataInfo.dataSize;
            node->binarySource = QSharedPointer<PboBinarySource>(
                new PboBinarySource(path, dataInfo));
        }

        binaryBackend_ = QSharedPointer<BinaryBackend>(new BinaryBackend);
        headers_ = header.headers;
    }

    void PboModel2::saveFile(const Cancel& cancel) {
        PboWriter writer;
        writer.usePath(file_->fileName() + ".t")
              .useRoot(rootEntry_.get());

        for (const QSharedPointer<PboHeader>& header : headers_) {
            writer.addHeader(header.get());
        }

        writer.write(cancel);
    }

    void PboModel2::unloadFile() {
        if (!rootEntry_)
            throw PboTreeException("The model is not initialized");

        file_->close();
        file_.clear();
        rootEntry_.clear();
    }

    void PboModel2::createNodeSet(PboNode* parent, const QList<NodeDescriptor>& descriptors,
                                  const ConflictsParcel& conflicts) const {
        if (!rootEntry_)
            throw PboTreeException("The model is not initialized");
        

        for (const NodeDescriptor& descriptor : descriptors) {
            const ConflictResolution resolution = conflicts.getResolution(descriptor);
            if (resolution != ConflictResolution::Skip) {
                PboNode* created = parent->createHierarchy(descriptor.path(), resolution);
                created->binarySource = descriptor.binarySource();
            }
        }
    }

    InteractionParcel PboModel2::interactionPrepare(const QList<PboNode*>& nodes, const Cancel& cancel) const {
        QList<QUrl> files = binaryBackend_->hddSync(nodes, cancel);
        NodeDescriptors descriptors = NodeDescriptors::packNodes(nodes);
        return InteractionParcel(std::move(files), std::move(descriptors));
    }

    ConflictsParcel PboModel2::checkConflicts(PboNode* parent, const QList<NodeDescriptor>& descriptors) const {
        if (!rootEntry_)
            throw PboTreeException("The model is not initialized");

        ConflictsParcel conflicts;
        for (const NodeDescriptor& descriptor : descriptors) {
            if (parent->get(PboPath(descriptor.path()))) {
                conflicts.setResolution(descriptor, ConflictResolution::Copy);
            }
        }
        return conflicts;
    }

    PboNode* PboModel2::rootEntry() const {
        return rootEntry_.get();
    }
}
