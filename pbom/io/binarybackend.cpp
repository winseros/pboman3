#include "binarybackend.h"
#include <QDir>
#include "pboioexception.h"

namespace pboman3 {
#define TEMP_PBOMAN "pboman3"

    BinaryBackend::BinaryBackend(const QString& name) {
        tree_ = TEMP_PBOMAN + static_cast<QString>(QDir::separator()) + name + QDir::separator() + "tree_";
        exec_ = TEMP_PBOMAN + static_cast<QString>(QDir::separator()) + name + QDir::separator() + "exec_";
        if (!QDir::temp().mkpath(tree_) || !QDir::temp().mkpath(exec_)) {
            throw PboIoException("Could not initialize the temporary folder");
        }
    }

    BinaryBackend::~BinaryBackend() {
        QDir temp = QDir::temp();
        temp.cd(TEMP_PBOMAN);
        assert(temp.removeRecursively() && "Normally the folder should have been removed");
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

    QString BinaryBackend::execSync(const PboNode* node, const Cancel& cancel) {
        if (execStore_.isNull())
            execStore_ = QSharedPointer<ExecStore>(new ExecStore(QDir::tempPath() + QDir::separator() + exec_));

        QString path = execStore_->execSync(node, cancel);

        return path;
    }

    QString BinaryBackend::syncPboFileNode(const PboNode* node, const Cancel& cancel) const {
        QString fsPath = makeFsPath(node->makePath(), tree_);

        const QFileInfo fi(fsPath);
        if (!fi.exists()) {

            const QString fsRelative = QDir::temp().relativeFilePath(fi.dir().absolutePath());
            if (!QDir::temp().mkpath(fsRelative))
                throw PboIoException("Could not create folder in temp: " + fsRelative);

            QFile file(fsPath);
            if (!file.open(QIODeviceBase::ReadWrite | QIODeviceBase::NewOnly))
                throw PboIoException("Could not open file: " + fsPath);

            node->binarySource->writeToFs(&file, cancel);

            file.close();

            if (cancel())
                file.remove();
        }

        return fsPath;
    }

    QString BinaryBackend::syncPboDirNode(const PboNode* node, const Cancel& cancel) const {
        syncPboDir(node, cancel);
        QString fsPath = makeFsPath(node->makePath(), tree_);
        return fsPath;
    }

    void BinaryBackend::syncPboDir(const PboNode* node, const Cancel& cancel) const {
        for (const PboNode* child : *node) {
            if (child->nodeType() == PboNodeType::File)
                syncPboFileNode(child, cancel);
            else
                syncPboDir(child, cancel);
        }

    }

    QString BinaryBackend::makeFsPath(const PboPath& pboPath, const QString& location) const {
        QString result = QDir::tempPath();
        constexpr int treeSegSeparatorsCount = 1;

        const auto length = result.length()
            + treeSegSeparatorsCount
            + location.length()
            + pboPath.length()
            + std::accumulate(pboPath.begin(), pboPath.end(), 0,
                              [](int acc, const QString& p) { return p.length(); });
        result.reserve(length);

        result.append("/");
        result.append(location);

        for (const QString& p : pboPath) {
            result.append("/");
            result.append(p);
        }

        return result;
    }
}
