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
        auto it = path.constBegin();
        const auto last = --path.constEnd();
        while (it != last) {
            const PboNode* folder = node->get(PboPath{*it});
            if (folder) {
                if (folder->nodeType() == PboNodeType::File)
                    return true;
                node = folder;
            } else {
                return false;
            }
            ++it;
        }

        const PboNode* file = node->get(PboPath{*last});
        return file;
    }

    const QString* GetPrefixValueUnsanitized(const DocumentHeaders& headers) {
        auto header = headers.begin();
        while (header != headers.end()) {
            if (header->name() == DocumentHeaders::PREFIX_HEADER_NAME) {
                return &header->value();
            }
            ++header;
        }
        return nullptr;
    }
}
