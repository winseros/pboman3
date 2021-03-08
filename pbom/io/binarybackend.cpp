#include "binarybackend.h"
#include <QDir>
#include <QSaveFile>
#include <QUuid>
#include "pboioexception.h"

namespace pboman3 {
    BinaryBackend::BinaryBackend() {
        tree_ = QUuid::createUuid().toString(QUuid::WithoutBraces) + "/tree_";
        if (!QDir::temp().mkpath(tree_)) {
            throw PboIoException("Could not initialize the temporary folder");
        }
    }

    BinaryBackend::~BinaryBackend() {
        assert(QDir::temp().rmdir(tree_) && "Normally the folder should have been removed");
    }

    QList<QUrl> BinaryBackend::hddSync(const QList<PboNode*>& nodes, const Cancel& cancel) const {
        QList<QUrl> result;
        for (const PboNode* node : nodes) {
            QString path = node->nodeType() == PboNodeType::File
                               ? syncPboFileNode(node, cancel)
                               : syncPboDirNode(node, cancel);
            result.append(QUrl::fromLocalFile(path));
        }
        return result;
    }

    QString BinaryBackend::syncPboFileNode(const PboNode* node, const Cancel& cancel) const {
        QString fsPath = makeFsPath(node->makePath());

        const QFileInfo fi(fsPath);
        if (!fi.exists()) {

            const QString fsRelative = QDir::temp().relativeFilePath(fi.dir().absolutePath());
            if (!QDir::temp().mkpath(fsRelative))
                throw PboIoException("Could not create folder in temp: " + fsRelative);

            QSaveFile file(fsPath);
            file.open(QIODeviceBase::WriteOnly);

            node->binarySource->writeDecompressed(&file, cancel);

            if (cancel())
                file.cancelWriting();
            else
                file.commit();
        }

        return fsPath;
    }

    QString BinaryBackend::syncPboDirNode(const PboNode* node, const Cancel& cancel) const {
        syncPboDir(node, cancel);
        QString fsPath = makeFsPath(node->makePath());
        return fsPath;
    }

    void BinaryBackend::syncPboDir(const PboNode* node, const Cancel& cancel) const {
        auto it = node->begin();
        while (it != node->end()) {
            if ((*it)->nodeType() == PboNodeType::File)
                syncPboFileNode(*it, cancel);
            else
                syncPboDir(*it, cancel);
            ++it;
        }
    }

    QString BinaryBackend::makeFsPath(const PboPath& pboPath) const {
        QString result = QDir::tempPath();
        constexpr int treeSegSeparatorsCount = 1;

        const auto length = result.length()
            + treeSegSeparatorsCount
            + tree_.length()
            + pboPath.length()
            + std::accumulate(pboPath.begin(), pboPath.end(), 0,
                              [](int acc, const QString& p) { return p.length(); });
        result.reserve(length);

        result.append("/");
        result.append(tree_);

        for (const QString& p : pboPath) {
            result.append("/");
            result.append(p);
        }

        return result;
    }
}
