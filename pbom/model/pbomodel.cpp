#include "pbomodel.h"
#include <QDir>
#include <QUrl>
#include <QUuid>
#include "diskaccessexception.h"
#include "pbofileformatexception.h"
#include "rootreader.h"
#include "io/pboheaderreader.h"
#include "io/pboioexception.h"
#include "io/pbowriter.h"
#include "util/log.h"

#define LOG(...) LOGGER("model/PboModel", __VA_ARGS__)

namespace pboman3 {
    void PboModel::loadFile(const QString& path) {
        LOG(info, "Loading the file:", path)

        if (!loadedPath_.isNull()) {
            LOG(info, "Unloading the previous file")
            unloadFile();
        }

        setLoadedPath(path);

        PboFile file(loadedPath_);
        if (!file.open(QIODeviceBase::OpenModeFlag::ReadWrite))
            throw DiskAccessException("Can not access the file. Check if it is used by other processes.", path);

        QString title = QFileInfo(path).fileName();
        LOG(info, "The file title is:", title)

        rootEntry_ = QSharedPointer<PboNode>(new PboNode(std::move(title), PboNodeType::Container, nullptr));
        connect(rootEntry_.get(), &PboNode::hierarchyChanged, this, &PboModel::modelChanged);
        connect(rootEntry_.get(), &PboNode::titleChanged, this, &PboModel::rootTitleChanged);

        PboFileHeader header;
        try {
            header = PboHeaderReader::readFileHeader(&file);
            LOG(info, "The file header:", header)
        } catch (const PboIoException& ex) {
            LOG(warning, "Got error while reading the file header:", ex)
            throw PboFileFormatException("Can not open the file. It is not a valid PBO.");
        }

        headers_ = QSharedPointer<HeadersModel>(new HeadersModel);
        headers_->setData(std::move(header.headers));
        connect(headers_.get(), &HeadersModel::changed, this, &PboModel::modelChanged);

        signature_ = QSharedPointer<SignatureModel>(new SignatureModel);
        signature_->setSignatureBytes(header.signature);

        RootReader(&header, path).inflateRoot(rootEntry_.get());

        LOG(info, "Creating the binary backend")
        binaryBackend_ = QSharedPointer<BinaryBackend>(
            new BinaryBackend(QUuid::createUuid().toString(QUuid::WithoutBraces)));
    }

    void PboModel::saveFile(const Cancel& cancel, const QString& filePath) {
        LOG(info, "Saving the model to:", filePath)

        const QString savePath = filePath.isNull() ? loadedPath_ : filePath;
        const QString tempPath(savePath + ".t");

        LOG(info, "The savePath was set as:", savePath)
        LOG(info, "The tempPath was set as:", tempPath)

        QByteArray signature;

        PboWriter writer;
        writer.usePath(savePath == loadedPath_ ? tempPath : savePath)
              .useHeaders(headers_.get())
              .useRoot(rootEntry_.get())
              .copySignatureTo(&signature);

        try {
            LOG(info, "Writing the file")
            writer.write(cancel);
        } catch (const PboIoException& ex) {
            LOG(warning, "Got error while writing:", ex)
            throw DiskAccessException(ex);
        }

        if (cancel()) {
            LOG(info, "The write process was canceled - exit")
            return;
        }

        LOG(info, "Clean up the previous binary sources")
        writer.suspendBinarySources();

        LOG(info, "Update the model signature")
        signature_->setSignatureBytes(signature);

        if (savePath == loadedPath_) {
            const QString backupPath = loadedPath_ + ".bak";
            LOG(info, "Cleaning up the temporary files")
            if (QFile::exists(backupPath) && !QFile::remove(backupPath)) {
                LOG(info, "Could not remove the prev backup file - throwing;", backupPath)
                writer.resumeBinarySources();
                throw DiskAccessException(
                    "Could not remove the file. Check you have enough permissions and the file is not locked by another process.",
                    backupPath);
            }
            if (!QFile::rename(loadedPath_, backupPath)) {
                LOG(info, "Could not replace the prev PBO file with a write copy - throwing;", loadedPath_)
                writer.resumeBinarySources();
                throw DiskAccessException(
                    "Could not write to the file. Check you have enough permissions and the file is not locked by another process.",
                    loadedPath_);
            }
            const bool renamed = QFile::rename(tempPath, loadedPath_);
            assert(renamed);
        }

        LOG(info, "Assign binary sources back")
        writer.assignBinarySources(savePath);

        setLoadedPath(savePath);
    }

    void PboModel::unloadFile() {
        if (!rootEntry_)
            throw InvalidOperationException("The model is not initialized");

        LOG(info, "Unloading the current file")

        setLoadedPath(nullptr);

        signature_.clear();
        headers_.clear();
        rootEntry_.clear();
        binaryBackend_.clear();
    }

    void PboModel::createNodeSet(PboNode* parent, const QList<NodeDescriptor>& descriptors,
                                 const ConflictsParcel& conflicts) const {
        if (!rootEntry_)
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

        QList<QUrl> files;
        try {
            files = binaryBackend_->hddSync(nodes, cancel);
            LOG(info, "Got files:", files)
        } catch (const PboIoException& ex) {
            LOG(warning, "Got error while syncing:", ex)
            throw DiskAccessException(ex);
        }

        NodeDescriptors descriptors = NodeDescriptors::packNodes(nodes);
        LOG(info, "Got descriptors:", descriptors)

        return InteractionParcel(std::move(files), std::move(descriptors));
    }

    QString PboModel::execPrepare(const PboNode* node, const Cancel& cancel) const {
        LOG(info, "Preparing the execution of the node", *node)

        QString file;
        try {
            file = binaryBackend_->execSync(node, cancel);
            LOG(info, "The node contents was stored to:", file)
        } catch (const PboIoException& ex) {
            LOG(warning, "Got error while syncing:", ex)
            throw DiskAccessException(ex);
        }

        return file;
    }

    ConflictsParcel PboModel::checkConflicts(const PboNode* parent, const QList<NodeDescriptor>& descriptors) const {
        if (!rootEntry_)
            throw InvalidOperationException("The model is not initialized");

        LOG(info, "Check conflicts for the set of descriptors")

        ConflictsParcel conflicts;
        for (const NodeDescriptor& descriptor : descriptors) {
            if (parent->isPathConflict(PboPath(descriptor.path()))) {
                LOG(info, "The descriptor is in conflict:", descriptor)
                conflicts.setResolution(descriptor, ConflictResolution::Copy);
            }
        }

        return conflicts;
    }

    void PboModel::unpackNodesTo(const QDir& dest, const PboNode* rootNode, const QList<PboNode*>& childNodes,
                                 const Cancel& cancel) const {
        try {
            LOG(info, "Unpack", childNodes.count(), "nodes to", dest)
            binaryBackend_->unpackSync(dest, rootNode, childNodes, cancel);
        } catch (const PboIoException& ex) {
            LOG(warning, "Got error while unpacking:", ex)
            throw DiskAccessException(ex);
        }
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
            LOG(info, "Set the loadedPath to:", loadedFile)
            loadedPath_ = loadedFile;
            emit loadedPathChanged();
        }
    }

    void PboModel::rootTitleChanged() {
        LOG(info, "The root title was changed")
        QFileInfo fi(loadedPath_);
        fi.setFile(fi.dir(), rootEntry_->title());
        setLoadedPath(fi.absoluteFilePath());
    }
}
