#include "binarybackend.h"
#include "unpackbackend.h"
#include "io/diskaccessexception.h"
#include "util/log.h"

#define LOG(...) LOGGER("io/bb/BinaryBackend", __VA_ARGS__)

namespace pboman3::io {
    using namespace domain;
#define TEMP_PBOMAN "pboman3"

    BinaryBackend::BinaryBackend(const QString& name) {
        const QString folder = TEMP_PBOMAN + static_cast<QString>(QDir::separator()) + name + QDir::separator();
        const QString treePath = folder + "tree_";
        const QString execPath = folder + "exec_";

        const QDir tree(QDir::temp().filePath(treePath));
        const QDir exec(QDir::temp().filePath(execPath));

        LOG(info, "Binary backend tree:", tree)
        LOG(info, "Binary backend exec:", exec)

        if (!QDir::temp().mkpath(treePath))
            throw DiskAccessException("Could not create the folder.", tree.path());
        if (!QDir::temp().mkpath(execPath))
            throw DiskAccessException("Could not create the folder.", exec.path());

        tempBackend_ = QSharedPointer<TempBackend>(new TempBackend(tree));
        execBackend_ = QSharedPointer<ExecBackend>(new ExecBackend(exec));
    }

    QList<QUrl> BinaryBackend::hddSync(const QList<PboNode*>& nodes, const Cancel& cancel) const {
        QList<QUrl> result = tempBackend_->hddSync(nodes, cancel);
        return result;
    }

    QString BinaryBackend::execSync(const PboNode* node, const Cancel& cancel) const {
        QString result = execBackend_->execSync(node, cancel);
        return result;
    }

    void BinaryBackend::unpackSync(const QDir& dest, const PboNode* rootNode, const QList<PboNode*>& childNodes,
                                   const Cancel& cancel) const {
        UnpackBackend unpack(dest);
        unpack.unpackSync(rootNode, childNodes, cancel);
    }

    void BinaryBackend::clear(const PboNode* node) const {
        tempBackend_->clear(node);
        execBackend_->clear(node);
    }
}
