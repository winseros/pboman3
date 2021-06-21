#pragma once

#include "pbonode.h"
#include "pbopath.h"
#include "io/bs/binarysource.h"
#include "io/bs/fslzhbinarysource.h"
#include "io/bs/fsrawbinarysource.h"
#include "io/bs/pbobinarysource.h"
#include <QDebug>

namespace pboman3 {
    enum class BinarySourceType {
        Pbo = 0,
        FsLzh = 1,
        FsRaw = 2
    };

    class NodeDescriptor {
    public:
        NodeDescriptor(const QSharedPointer<BinarySource>& binarySource,
                       PboPath path)
            : binarySource_(binarySource),
              path_(std::move(path)) {
        }

        const QSharedPointer<BinarySource>& binarySource() const;

        const PboPath& path() const;

        void setCompressed(bool compressed);

        friend QDebug operator <<(QDebug debug, const NodeDescriptor& descriptor) {
            return debug << "NodeDescriptor(PboPath=" << descriptor.path() << ", BinarySource=" << *descriptor.
                binarySource() << ")";
        }

    private:
        QSharedPointer<BinarySource> binarySource_;
        PboPath path_;
    };


    class NodeDescriptors : public QList<NodeDescriptor> {
    public:
        static QByteArray serialize(const NodeDescriptors& data);

        static NodeDescriptors deserialize(const QByteArray& data);

        static NodeDescriptors packNodes(const QList<PboNode*>& nodes);

    private:
        static void writeNodeInfo(QDataStream& stream, const NodeDescriptor& nodeInfo);

        static void writeBinarySource(QDataStream& stream, const PboBinarySource* bs);

        static void writeBinarySource(QDataStream& stream, const FsLzhBinarySource* bs);

        static void writeBinarySource(QDataStream& stream, const FsRawBinarySource* bs);

        static NodeDescriptor readNodeInfo(QDataStream& stream);

        static QSharedPointer<BinarySource> readPboBinarySource(QDataStream& stream);

        static QSharedPointer<BinarySource> readFsLzhBinarySource(QDataStream& stream);

        static QSharedPointer<BinarySource> readFsRawBinarySource(QDataStream& stream);

        static void addNodeToParcel(NodeDescriptors& descriptors, PboNode* node, const PboPath& parentPath,
                                    QSet<PboNode*>& dedupe);
    };

    class InteractionParcel {
    public:
        InteractionParcel(QList<QUrl> files, NodeDescriptors nodes)
            : files_(std::move(files)),
              nodes_(std::move(nodes)) {
        }

        const QList<QUrl>& files() const {
            return files_;
        }

        const NodeDescriptors& nodes() const {
            return nodes_;
        }

        friend QDebug operator<<(QDebug debug, const InteractionParcel& parcel);

    private:
        QList<QUrl> files_;
        NodeDescriptors nodes_;
    };
}
