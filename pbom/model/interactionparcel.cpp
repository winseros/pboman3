#include "interactionparcel.h"
#include <QDataStream>
#include <QSet>
#include "util/appexception.h"

namespace pboman3 {
    const QSharedPointer<BinarySource>& NodeDescriptor::binarySource() const {
        return binarySource_;
    }

    const PboPath& NodeDescriptor::path() const {
        return path_;
    }

    void NodeDescriptor::setCompressed(bool compressed) {
        if (dynamic_cast<PboBinarySource*>(binarySource_.get())) {
            throw AppException("Can't query compression status");
        }

        if (compressed) {
            if (dynamic_cast<FsRawBinarySource*>(binarySource_.get()))
                binarySource_ = QSharedPointer<BinarySource>(new FsLzhBinarySource(binarySource_->path()));
        } else {
            if (dynamic_cast<FsLzhBinarySource*>(binarySource_.get()))
                binarySource_ = QSharedPointer<BinarySource>(new FsRawBinarySource(binarySource_->path()));
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

        return NodeDescriptor(bs, path);
    }

    QSharedPointer<BinarySource> NodeDescriptors::readPboBinarySource(QDataStream& stream) {
        QString fsPath;
        stream >> fsPath;
        int originalSize;
        stream >> originalSize;
        int dataSize;
        stream >> dataSize;
        size_t dataOffset;
        stream >> dataOffset;

        const PboDataInfo info(originalSize, dataSize, dataOffset);
        return QSharedPointer<BinarySource>(new PboBinarySource(fsPath, info));
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

    NodeDescriptors NodeDescriptors::packTree(PboNode* root, const QList<PboPath>& paths) {
        NodeDescriptors descriptors;
        descriptors.reserve(paths.length() * 2);

        QList<PboNode*> files;
        files.reserve(paths.length());

        QSet<PboNode*> dedupe;
        dedupe.reserve(paths.length() * 2);

        //the input list of paths may contain something like:
        //[folder1\file1.txt, folder1\]
        //thus referencing file1.txt twice
        //while in the resulting parcel we must not have duplicates
        //so we do the following:
        //1) handle folders first, starting from the top-level ones
        //2) handle standalone files after
        //that allows to avoid duplicates in the parcel

        QList<PboPath> sortedPaths(paths);
        std::sort(sortedPaths.begin(), sortedPaths.end(), [](const PboPath& a, const PboPath& b) {
            return a.length() < b.length();
        });

        for (const PboPath& path : paths) {
            PboNode* node = root->get(path);
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
            auto it = node->begin();
            while (it != node->end()) {
                addNodeToParcel(descriptors, *it, nodePath, dedupe);
                ++it;
            }
        }
    }
}
