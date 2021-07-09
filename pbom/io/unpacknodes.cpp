#include "unpacknodes.h"
#include "io/pboioexception.h"
#include "util/exception.h"
#include "util/log.h"

#define LOG(...) LOGGER("io/UnpackNodes", __VA_ARGS__)

namespace pboman3 {
    void UnpackNodes::unpackTo(const QString& dir, const PboNode* rootNode, const QList<PboNode*>& childNodes,
                               const Cancel& cancel) {
        if (!QDir(dir).exists()) {
            LOG(critical, "The directory does not exist:", dir)
            throw InvalidOperationException("The target directory does not exist");
        }

        for (PboNode* childNode : childNodes) {
            if (cancel()) {
                LOG(info, "The extraction was cancelled - exiting")
                break;
            }

            unpackNode(dir, rootNode, childNode, cancel);
        }
    }

    void UnpackNodes::unpackNode(const QString& dir, const PboNode* rootNode, const PboNode* childNode,
                                 const Cancel& cancel) {
        if (childNode->nodeType() == PboNodeType::File)
            unpackFileNode(dir, rootNode, childNode, cancel);
        else
            unpackFolderNode(dir, rootNode, childNode, cancel);
    }

    void UnpackNodes::unpackFolderNode(const QString& dir, const PboNode* rootNode, const PboNode* childNode,
                                       const Cancel& cancel) {
        for (const PboNode* child : *childNode) {
            if (cancel()) {
                LOG(info, "The extraction was cancelled - exiting")
                break;
            }
            unpackNode(dir, rootNode, child, cancel);
        }
    }

    void UnpackNodes::unpackFileNode(const QString& dir, const PboNode* rootNode, const PboNode* childNode,
                                     const Cancel& cancel) {
        LOG(info, "Unpack the node", childNode->title(), "to dir", dir)

        const QString filePath = createPathForFile(dir, rootNode, childNode);
        if (cancel())
            return;

        QFile file(filePath);
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

    QString UnpackNodes::createPathForFile(const QString& dir, const PboNode* rootNode, const PboNode* childNode) {
        QList<QString> pathSegs;
        pathSegs.reserve(childNode->depth() - rootNode->depth());
        PboNode* parent = childNode->parentNode();
        while (parent && parent != rootNode) {
            pathSegs.prepend(parent->title());
            parent = parent->parentNode();
        }
        if (!parent) {
            LOG(critical, "The provided rootNode is not a real parent of the provided childNode")
            throw InvalidOperationException("The provided rootNode is not a real parent of the provided childNode");
        }

        QDir dirObj(dir);
        for (const QString& pathSeg : pathSegs) {
            if (!QDir(dirObj.filePath(pathSeg)).exists() && !dirObj.mkdir(pathSeg)) {
                LOG(critical, "Could not create the folder:", dirObj.absolutePath())
                throw PboIoException("Could not create the folder. Check you have enough permissions.",
                                     dirObj.absolutePath());
            }
            dirObj.cd(pathSeg);
        }

        return dirObj.filePath(childNode->title());
    }
}
