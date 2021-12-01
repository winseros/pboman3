#pragma once

#include "pbonode.h"

namespace pboman3::domain {
    void CountFilesInTree(const PboNode& node, qint32& result);

    bool IsPathConflict(const PboNode* node, const PboPath& path);

    const PboNode* FindDirectChild(const PboNode* parent, const QString& title);

    PboNode* FindDirectChild(PboNode* parent, const QString& title);
}
