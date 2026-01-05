#pragma once

#include "pbonode.h"
#include "documentheaders.h"

namespace pboman3::domain {
    void CountFilesInTree(const PboNode& node, qint32& result);

    bool IsPathConflict(const PboNode* node, const PboPath& path);

    const QString* GetPrefixValue(const DocumentHeaders& headers);
}
