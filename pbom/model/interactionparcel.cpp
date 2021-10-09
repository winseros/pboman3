#include "interactionparcel.h"
#include <QDataStream>
#include <QSet>
#include "util/exception.h"

namespace pboman3 {
    const QSharedPointer<BinarySource>& NodeDescriptor::binarySource() const {
        return binarySource_;
    }

    const PboPath& NodeDescriptor::path() const {
        return path_;
    }

    void NodeDescriptor::setCompressed(bool compressed) {
        if (dynamic_cast<PboBinarySource*>(binarySource_.get())) {
            throw InvalidOperationException("Can't query compression status");
        }

        if (compressed) {
            if (dynamic_cast<FsRawBinarySource*>(binarySource_.get())) {
                binarySource_ = QSharedPointer<BinarySource>(new FsLzhBinarySource(binarySource_->path()));
                binarySource_->open();
            }
        } else {
            if (dynamic_cast<FsLzhBinarySource*>(binarySource_.get())) {
                binarySource_ = QSharedPointer<BinarySource>(new FsRawBinarySource(binarySource_->path()));
                binarySource_->open();
            }
        }
    }

    QByteArray NodeDescriptors::serialize(const NodeDescriptors& data) {
        QByteArray result;
        result.reserve(1024); //just some arbitrary value

        QDataStream stream(&result, QIODeviceBase::ReadWrite);
        stream << static_cast<qint16>(data.length());

        for (const NodeDescriptor& nodeInfo : data) {
            writeNodeInfo(stream, nodeInfo);
        }

        return result;
    }

    void NodeDescriptors::writeNodeInfo(QDataStream& stream, const NodeDescriptor& nodeInfo) {
        stream << nodeInfo.path();
        const BinarySource* bs = nodeInfo.binarySource().get();
        if (const auto* bsPbo = dynamic_cast<const PboBinarySource*>(bs)) {
            writeBinarySource(stream, bsPbo);
        } else if (const auto* fsLzh = dynamic_cast<const FsLzhBinarySource*>(bs)) {
            writeBinarySource(stream, fsLzh);
        } else if (const auto* bsFs = dynamic_cast<const FsRawBinarySource*>(bs)) {
            writeBinarySource(stream, bsFs);
        } else {
            assert(false && "Unknown binary source type");
        }
    }

    void NodeDescriptors::writeBinarySource(QDataStream& stream, const PboBinarySource* bs) {
        stream << static_cast<int8_t>(BinarySourceType::Pbo);
        stream << bs->path();
        const PboDataInfo info = bs->getInfo();
        stream << info.originalSize;
        stream << info.dataSize;
        stream << info.dataOffset;
        stream << info.timestamp;
        stream << info.compressed;
    }

    void NodeDescriptors::writeBinarySource(QDataStream& stream, const FsLzhBinarySource* bs) {
        stream << static_cast<int8_t>(BinarySourceType::FsLzh);
        stream << bs->path();
    }

    void NodeDescriptors::writeBinarySource(QDataStream& stream, const FsRawBinarySource* bs) {
        stream << static_cast<int8_t>(BinarySourceType::FsRaw);
        stream << bs->path();
    }

    NodeDescriptors NodeDescriptors::deserialize(const QByteArray& data) {
        QDataStream stream(data);
        qint16 length;
        stream >> length;

        NodeDescriptors result;
        result.reserve(length);

        for (short i = 0; i < length; i++) {
            NodeDescriptor nodeInfo = readNodeInfo(stream);
            result.append(nodeInfo);
        }

        return result;
    }

    NodeDescriptor NodeDescriptors::readNodeInfo(QDataStream& stream) {
        PboPath path;
        stream >> path;

        int8_t sourceType;
        stream >> sourceType;

        QSharedPointer<BinarySource> bs;
        switch (static_cast<BinarySourceType>(sourceType)) {
            case BinarySourceType::Pbo: {
                bs = readPboBinarySource(stream);
                break;
            }
            case BinarySourceType::FsLzh: {
                bs = readFsLzhBinarySource(stream);
                break;
            }
            case BinarySourceType::FsRaw: {
                bs = readFsRawBinarySource(stream);
                break;
            }
        }
        bs->open();

        return NodeDescriptor(bs, path);
    }

    QSharedPointer<BinarySource> NodeDescriptors::readPboBinarySource(QDataStream& stream) {
        QString fsPath;
        stream >> fsPath;

        PboDataInfo di;  // NOLINT(cppcoreguidelines-pro-type-member-init)
        stream >> di.originalSize;
        stream >> di.dataSize;
        stream >> di.dataOffset;
        stream >> di.timestamp;
        stream >> di.compressed;

        return QSharedPointer<BinarySource>(new PboBinarySource(fsPath, di));
    }

    QSharedPointer<BinarySource> NodeDescriptors::readFsLzhBinarySource(QDataStream& stream) {
        QString fsPath;
        stream >> fsPath;
        return QSharedPointer<BinarySource>(new FsLzhBinarySource(fsPath));
    }

    QSharedPointer<BinarySource> NodeDescriptors::readFsRawBinarySource(QDataStream& stream) {
        QString fsPath;
        stream >> fsPath;
        return QSharedPointer<BinarySource>(new FsRawBinarySource(fsPath));
    }

    NodeDescriptors NodeDescriptors::packNodes(const QList<PboNode*>& nodes) {
        NodeDescriptors descriptors;
        descriptors.reserve(nodes.length() * 2);

        QList<PboNode*> files;
        files.reserve(nodes.length());

        QSet<PboNode*> dedupe;
        dedupe.reserve(nodes.length());

        //the input list of paths may contain something like:
        //[folder1\file1.txt, folder1\]
        //thus referencing file1.txt twice
        //while in the resulting parcel we must not have duplicates
        //so we do the following:
        //1) handle folders first, starting from the top-level ones
        //2) handle standalone files after
        //that allows to avoid duplicates in the parcel

        QList<PboNode*> sortedNodes(nodes);
        std::sort(sortedNodes.begin(), sortedNodes.end(), [](const PboNode* a, const PboNode* b) {
            return a->depth() < b->depth();
        });

        for (PboNode* node : sortedNodes) {
            if (node->nodeType() == PboNodeType::File) {
                files.append(node);
            } else {
                addNodeToParcel(descriptors, node, PboPath(), dedupe);
            }
        }

        for (PboNode* file : files) {
            addNodeToParcel(descriptors, file, PboPath(), dedupe);
        }

        return descriptors;
    }

    void NodeDescriptors::addNodeToParcel(NodeDescriptors& descriptors, PboNode* node, const PboPath& parentPath,
                                          QSet<PboNode*>& dedupe) {
        if (node->nodeType() == PboNodeType::File) {
            if (!dedupe.contains(node)) {
                dedupe.insert(node);
                PboPath nodePath = parentPath.makeChild(node->title());
                descriptors.append(NodeDescriptor(node->binarySource, std::move(nodePath)));
            }
        } else {
            const PboPath nodePath = parentPath.makeChild(node->title());
            for (PboNode* child : *node) {
                addNodeToParcel(descriptors, child, nodePath, dedupe);
            }
        }
    }

    QDebug operator<<(QDebug debug, const InteractionParcel& parcel) {
        return debug << "InteractionParcel(Files=" << parcel.files_.count()
            << ", Nodes=" << parcel.nodes_.count() << ")";
    }
}
