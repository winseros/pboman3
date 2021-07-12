#include "unpackbackend.h"
#include <QDir>
#include "io/pboioexception.h"
#include "util/exception.h"
#include "util/log.h"

#define LOG(...) LOGGER("io/bb/UnpackBackend", __VA_ARGS__)

namespace pboman3 {
    UnpackBackend::UnpackBackend(const QDir& folder) {
        if (!folder.exists())
            throw InvalidOperationException("The folder provided must exist");
        nodeFileSystem_ = QSharedPointer<NodeFileSystem>(new NodeFileSystem(folder));
    }

    void UnpackBackend::unpackSync(const PboNode* rootNode, const QList<PboNode*>& childNodes,
                                 const Cancel& cancel) {
        assert(rootNode);

        LOG(info, "Unpack", childNodes.count(), "nodes")

        for (PboNode* childNode : childNodes) {
            if (cancel()) {
                LOG(info, "The extraction was cancelled - exiting")
                break;
            }

            unpackNode(rootNode, childNode, cancel);
        }
    }

    void UnpackBackend::unpackNode(const PboNode* rootNode, const PboNode* childNode,
                                   const Cancel& cancel) {
        if (childNode->nodeType() == PboNodeType::File)
            unpackFileNode(rootNode, childNode, cancel);
        else
            unpackFolderNode(rootNode, childNode, cancel);
    }

    void UnpackBackend::unpackFolderNode(const PboNode* rootNode, const PboNode* childNode,
                                         const Cancel& cancel) {
        for (const PboNode* child : *childNode) {
            if (cancel()) {
                LOG(info, "The extraction was cancelled - exiting")
                break;
            }
            unpackNode(rootNode, child, cancel);
        }
    }

    void UnpackBackend::unpackFileNode(const PboNode* rootNode, const PboNode* childNode,
                                       const Cancel& cancel) const {
        LOG(info, "Unpack the node", childNode->title())

        const QString filePath = nodeFileSystem_->allocatePath(rootNode, childNode);
        if (cancel())
            return;

        QFile file(filePath); //WriteOnly won't work for LZH unpacking
        if (!file.open(QIODeviceBase::ReadWrite)) {
            LOG(critical, "Can not access the file:", file.fileName())
            throw PboIoException(
                "Can not open the file. Check you have enough permissions and the file is not locked by another process.",
                file.fileName());
        }

        LOG(info, "Writing to file system")
        childNode->binarySource->writeToFs(&file, cancel);

        file.close();
    }
}
