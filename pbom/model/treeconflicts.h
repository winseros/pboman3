# pragma once

#include <QSet>
#include "pbonode.h"

namespace pboman3 {
    class TreeConflicts : public QHash<QString, TreeConflictResolution> {
    public:
        TreeConflicts():
            QHash() {
        }

        void inspect(const PboNode* node, const QString& path);

        TreeConflictResolution getResolution(const QString& path);
    };

    typedef std::function<void(TreeConflicts&)> ResolveConflictsFn;
}
