#include "nodefilesystem.h"
#include "io/pboioexception.h"
#include "util/log.h"

#define LOG(...) LOGGER("io/bb/NodeFileSystem", __VA_ARGS__)

namespace pboman3 {
    NodeFileSystem::NodeFileSystem(const QDir& folder)
        : QObject(),
          folder_(folder) {
    }

    QString NodeFileSystem::allocatePath(const PboNode* node) const {
        assert(node);

        QDir local(folder_);
        const PboPath path = node->makePath();
        auto it = path.begin();
        const auto last = path.end() - 1;

        while (it != last) {
            if (!local.exists(*it) && !local.mkdir(*it))
                throw PboIoException("Could not create the folder.", local.filePath(*it));
            local.cd(*it);
            ++it;
        }

        return local.filePath(*last);
    }

    QString NodeFileSystem::allocatePath(const PboNode* parent, const PboNode* node) const {
        assert(parent);
        assert(node);

        QList<QString> pathSegs;
        pathSegs.reserve(node->depth() - parent->depth());
        PboNode* p = node->parentNode();
        while (p && p != parent) {
            pathSegs.prepend(p->title());
            p = p->parentNode();
        }
        if (!p) {
            LOG(critical, "The provided rootNode is not a real parent of the provided childNode")
            throw InvalidOperationException("The provided rootNode is not a real parent of the provided childNode");
        }

        QDir dir(folder_);
        for (const QString& pathSeg : pathSegs) {
            if (!QDir(dir.filePath(pathSeg)).exists() && !dir.mkdir(pathSeg)) {
                LOG(critical, "Could not create the folder:", dir.absolutePath())
                throw PboIoException("Could not create the folder. Check you have enough permissions.",
                                     dir.absolutePath());
            }
            dir.cd(pathSeg);
        }

        return dir.filePath(node->title());
    }


    QString NodeFileSystem::composeAbsolutePath(const PboNode* node) const {
        QString fs = folder_.absolutePath();
        const PboPath path = node->makePath();

        auto it = path.begin();
        const auto last = path.end() - 1;

        if (it == last) {
            fs = fs + QDir::separator();
        } else {
            while (it != last) {
                fs = fs + *it + QDir::separator();
                ++it;
            }
        }
        fs = fs + *last;

        return fs;
    }

    QString NodeFileSystem::composeRelativePath(const PboNode* node) const {
        QString fs = "";
        const PboPath path = node->makePath();

        auto it = path.begin();
        const auto last = path.end() - 1;

        if (it == last) {
            fs = fs + QDir::separator();
        } else {
            while (it != last) {
                fs = fs + *it + QDir::separator();
                ++it;
            }
        }
        fs = fs + *last;

        return fs;
    }

    QDir NodeFileSystem::makeFsPath(const PboNode* node) const {
        assert(node);

        QDir local(folder_);
        const PboPath path = node->makePath();
        auto it = path.begin();
        const auto last = path.end() - 1;

        while (it != last) {
            local.cd(*it);
            ++it;
        }

        return local;
    }
}
