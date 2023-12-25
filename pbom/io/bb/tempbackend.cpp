#include "tempbackend.h"
#include <QDir>
#include "io/diskaccessexception.h"

namespace pboman3::io {
    using namespace domain;

    TempBackend::TempBackend(const QDir& folder)
        : folder_(folder) {
        if (!folder.exists())
            throw InvalidOperationException("The folder provided must exist");
        nodeFileSystem_ = QSharedPointer<NodeFileSystem>(new NodeFileSystem(folder));
    }

    TempBackend::~TempBackend() {
        folder_.removeRecursively();
    }

    QList<QUrl> TempBackend::hddSync(const QList<PboNode*>& nodes, const Cancel& cancel) const {
        QList<QUrl> result;
        for (const PboNode* node : nodes) {
            QString path = node->nodeType() == PboNodeType::File
                               ? syncPboFileNode(node, cancel)
                               : syncPboDirNode(node, cancel);
            result.append(QUrl::fromLocalFile(path));
        }
        return result;
    }


    void TempBackend::clear(const PboNode* node) const {
        assert(node->nodeType() == PboNodeType::File);

        const QString path = nodeFileSystem_->composeAbsolutePath(node);
        if (QFileInfo(path).exists()) {
            if (!QFile::remove(path)) {
                throw DiskAccessException(
                    "Could not remove the file. Check you have enough permissions and the file is not locked by another process.",
                    path);
            }
        }
    }

    QString TempBackend::syncPboFileNode(const PboNode* node, const Cancel& cancel) const {
        QString fsPath = nodeFileSystem_->allocatePath(node);

        if (const QFileInfo fi(fsPath); !fi.exists()) {
            QFile file(fsPath);
            if (!file.open(QIODeviceBase::ReadWrite | QIODeviceBase::NewOnly))
                throw DiskAccessException(
                    "Could not open the file. Check you have enough permissions and the file is not locked by another process.",
                    fsPath);

            const auto bsClose = qScopeGuard([&node]{if (node->binarySource->isOpen()) node->binarySource->close();});
            node->binarySource->open();
            node->binarySource->writeToFs(&file, cancel);

            file.close();

            if (cancel())
                file.remove();
        }

        return fsPath;
    }

    QString TempBackend::syncPboDirNode(const PboNode* node, const Cancel& cancel) const {
        syncPboDir(node, cancel);
        QString fsPath = nodeFileSystem_->composeAbsolutePath(node);
        return fsPath;
    }

    void TempBackend::syncPboDir(const PboNode* node, const Cancel& cancel) const {
        for (const PboNode* child : *node) {
            if (child->nodeType() == PboNodeType::File)
                // ReSharper disable once CppExpressionWithoutSideEffects
                syncPboFileNode(child, cancel);
            else
                syncPboDir(child, cancel);
        }
    }
}
