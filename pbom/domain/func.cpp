#include "func.h"

namespace pboman3::domain {
    void CountFilesInTree(const PboNode& node, qint32& result) {
        if (node.nodeType() == PboNodeType::File) {
            result++;
        } else {
            for (const PboNode* child : node)
                CountFilesInTree(*child, result);
        }
    }

    bool IsPathConflict(const PboNode* node, const PboPath& path) {
        const PboNode* n = node;
        for (qsizetype i = 0; i < path.length() - 1; i++) {
            const PboNode* folder = FindDirectChild(n, path.at(i));
            if (folder) {
                if (folder->nodeType() == PboNodeType::File)
                    return true;
                n = folder;
            } else {
                return false;
            }
        }

        const PboNode* file = FindDirectChild(n, path.last());
        return file;
    }

    const PboNode* FindDirectChild(const PboNode* parent, const QString& title) {
        for (const PboNode* child : *parent) {
            if (child->title().compare(title, Qt::CaseInsensitive) == 0)
                return child;
        }
        return nullptr;
    }

    PboNode* FindDirectChild(PboNode* parent, const QString& title) {
        for (PboNode* child : *parent) {
            if (child->title().compare(title, Qt::CaseInsensitive) == 0)
                return child;
        }
        return nullptr;
    }
}
