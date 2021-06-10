#include "pbomodel.h"
#include <QDir>
#include <QUrl>
#include <QUuid>
#include "pbotreeexception.h"
#include "io/pboheaderreader.h"
#include "io/pbowriter.h"
#include "io/bs/pbobinarysource.h"

namespace pboman3 {
    void PboModel::loadFile(const QString& path) {
        if (!loadedPath_.isNull())
            unloadFile();

        setLoadedPath(path);

        PboFile file(loadedPath_);
        file.open(QIODeviceBase::OpenModeFlag::ReadWrite);

        QString title = QFileInfo(path).fileName();
        rootEntry_ = QSharedPointer<PboNode>(new PboNode(std::move(title), PboNodeType::Container, nullptr));
        connect(rootEntry_.get(), &PboNode::hierarchyChanged, this, &PboModel::modelChanged);
        connect(rootEntry_.get(), &PboNode::titleChanged, this, &PboModel::rootTitleChanged);

        PboFileHeader header = PboHeaderReader::readFileHeader(&file);

        headers_ = QSharedPointer<HeadersModel>(new HeadersModel);
        headers_->setData(std::move(header.headers));
        connect(headers_.get(), &HeadersModel::changed, this, &PboModel::modelChanged);

        signature_ = QSharedPointer<SignatureModel>(new SignatureModel);
        signature_->setSignatureBytes(header.signature);

        size_t entryDataOffset = header.dataBlockStart;
        for (const QSharedPointer<PboEntry>& entry : header.entries) {
            PboNode* node = rootEntry_->createHierarchy(entry->makePath());
            PboDataInfo dataInfo{0, 0, 0, 0, 0};
            dataInfo.originalSize = entry->originalSize();
            dataInfo.dataSize = entry->dataSize();
            dataInfo.dataOffset = entryDataOffset;
            dataInfo.timestamp = entry->timestamp();
            dataInfo.compressed = entry->packingMethod() == PboPackingMethod::Packed;
            entryDataOffset += dataInfo.dataSize;
            node->binarySource = QSharedPointer<PboBinarySource>(
                new PboBinarySource(path, dataInfo));
        }

        binaryBackend_ = QSharedPointer<BinaryBackend>(
            new BinaryBackend(QUuid::createUuid().toString(QUuid::WithoutBraces)));
    }

    void PboModel::saveFile(const Cancel& cancel, const QString& filePath) {
        const QString savePath = filePath.isNull() ? loadedPath_ : filePath;
        const QString tempPath(savePath + ".t");

        QByteArray signature;

        PboWriter writer;
        writer.usePath(savePath == loadedPath_ ? tempPath : savePath)
              .useHeaders(headers_.get())
              .useRoot(rootEntry_.get())
              .copySignatureTo(&signature);

        writer.write(cancel);

        if (cancel())
            return;

        writer.cleanBinarySources();
        signature_->setSignatureBytes(signature);

        if (savePath == loadedPath_) {
            QFile::remove(loadedPath_ + ".bak");
            QFile::rename(loadedPath_, loadedPath_ + ".bak");
            QFile::rename(tempPath, loadedPath_);
        }

        writer.reassignBinarySources(savePath);
        setLoadedPath(savePath);
    }

    void PboModel::unloadFile() {
        if (!rootEntry_)
            throw PboTreeException("The model is not initialized");

        setLoadedPath(nullptr);

        signature_.clear();
        headers_.clear();
        rootEntry_.clear();
        binaryBackend_.clear();
    }

    void PboModel::createNodeSet(PboNode* parent, const QList<NodeDescriptor>& descriptors,
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

    InteractionParcel PboModel::interactionPrepare(const QList<PboNode*>& nodes, const Cancel& cancel) const {
        QList<QUrl> files = binaryBackend_->hddSync(nodes, cancel);
        NodeDescriptors descriptors = NodeDescriptors::packNodes(nodes);
        return InteractionParcel(std::move(files), std::move(descriptors));
    }

    ConflictsParcel PboModel::checkConflicts(PboNode* parent, const QList<NodeDescriptor>& descriptors) const {
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

    PboNode* PboModel::rootEntry() const {
        return rootEntry_.get();
    }

    HeadersModel* PboModel::headers() const {
        return headers_.get();
    }

    SignatureModel* PboModel::signature() const {
        return signature_.get();
    }

    const QString& PboModel::loadedPath() const {
        return loadedPath_;
    }

    void PboModel::setLoadedPath(const QString& loadedFile) {
        if (loadedPath_ != loadedFile) {
            loadedPath_ = loadedFile;
            emit loadedPathChanged();
        }
    }

    void PboModel::rootTitleChanged() {
        QFileInfo fi(loadedPath_);
        fi.setFile(fi.dir(), rootEntry_->title());
        setLoadedPath(fi.absoluteFilePath());
    }
}
