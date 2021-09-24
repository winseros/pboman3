#include "nodefilesystem.h"
#include "io/pboioexception.h"
#include "util/log.h"
#include "util/util.h"

#define LOG(...) LOGGER("io/bb/NodeFileSystem", __VA_ARGS__)

namespace pboman3 {
    NodeFileSystem::NodeFileSystem(const QDir& folder)
        : QObject(),
          folder_(folder) {
    }

    QString NodeFileSystem::allocatePath(const PboNode* node) const {
        assert(node);

        const QList<const PboNode*> parents = getParents(node);
        QString path = allocatePath(parents, node);

        return path;
    }

    QString NodeFileSystem::allocatePath(const PboNode* parent, const PboNode* node) const {
        assert(parent);
        assert(node);

        QList<const PboNode*> parents;
        parents.reserve(node->depth() - parent->depth());

        PboNode* p = node->parentNode();
        while (p && p != parent) {
            parents.prepend(p);
            p = p->parentNode();
        }
        if (!p) {
            LOG(critical, "The provided rootNode is not a real parent of the provided childNode")
            throw InvalidOperationException("The provided rootNode is not a real parent of the provided childNode");
        }

        QString path = allocatePath(parents, node);

        return path;
    }

    QString NodeFileSystem::composeAbsolutePath(const PboNode* node) const {
        const QString fs = folder_.absolutePath() + QDir::separator();
        QString path = composePath(node, fs);
        return path;
    }

    QString NodeFileSystem::composeRelativePath(const PboNode* node) const {
        QString path = composePath(node, "");
        return path;
    }

    QList<const PboNode*> NodeFileSystem::getParents(const PboNode* node) const {
        QList<const PboNode*> parents;
        parents.reserve(node->depth());
        const PboNode* p = node->parentNode();
        while (p->parentNode()) {
            parents.prepend(p);
            p = p->parentNode();
        }
        return parents;
    }

    QString NodeFileSystem::allocatePath(const QList<const PboNode*>& parents, const PboNode* node) const {
        QDir local(folder_);
        for (const PboNode* par : parents) {
            if (!local.exists(par->title()) && !local.mkdir(par->title()))
                throw PboIoException("Could not create the folder.", local.filePath(node->title()));
            local.cd(par->title());
        }
        
        return local.filePath(node->title());
    }

    QString NodeFileSystem::composePath(const PboNode* node, const QString& rootPath) const {
        QString fs = rootPath;

        QList<const PboNode*> parents = getParents(node);

        QDir local(folder_);
        for (const PboNode* par : parents) {
            fs.append(par->title()).append(QDir::separator());
            local.cd(par->title());
        }
        
        fs.append(node->title());

        return fs;
    }
}
