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

    const QString* GetPrefixValue(const DocumentHeaders& headers) {
        auto header = headers.begin();
        while (header != headers.end()) {
            if (header->name() == DocumentHeaders::PREFIX_HEADER_NAME) {
                return &header->value();
                /*const PboPath path(header->value());

                PboPath sanitizedPath;
                auto pathSeg = path.begin();
                while (pathSeg != path.end()) {
                    io::SanitizedString sanitizedSeg(*pathSeg);
                    sanitizedPath = sanitizedPath.makeChild(sanitizedSeg);
                    ++pathSeg;
                }

                return sanitizedPath.toString();*/
            }
            ++header;
        }
        return nullptr;
    }
}
