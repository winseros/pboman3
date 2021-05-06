#pragma once

#include "pbonode.h"
#include "treeconflicts.h"

namespace pboman3 {
    class ParcelManager {
    public:
        PboParcel packTree(const PboNode& root, const QList<PboPath>& paths) const;

        void unpackTree(QPointer<PboNode>& parent, const PboParcel& parcel, const ResolveConflictsFn& onConflict) const;

    private:
        void addNodeToParcel(PboParcel& parcel, const PboNode* node, const QString& parentPath,
                             QSet<const PboNode*>& dedupe) const;
    };
}
