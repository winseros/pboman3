#include "treeconflicts.h"

namespace pboman3 {

    void TreeConflicts::inspect(const PboNode* node, const QString& path) {
        const PboPath pboPath(path);
        if (node->fileExists(pboPath)) {
            insert(path, TreeConflictResolution::Copy);
        }
    }

    TreeConflictResolution TreeConflicts::getResolution(const QString& path) {
        const auto it = find(path);
        return it == end() ? TreeConflictResolution::Throw : *it;
    }
}
