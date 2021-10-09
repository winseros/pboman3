#pragma once

#include "nodefilesystem.h"
#include "model/pbonode.h"

namespace pboman3 {
    class UnpackBackend {
    public:
        explicit UnpackBackend(const QDir& folder);

        virtual ~UnpackBackend() = default;

        void unpackSync(const PboNode* rootNode, const QList<PboNode*>& childNodes, const Cancel& cancel);

    private:
        void unpackNode(const PboNode* rootNode, const PboNode* childNode, const Cancel& cancel);

        void unpackFolderNode(const PboNode* rootNode, const PboNode* childNode, const Cancel& cancel);

    protected:
        QSharedPointer<NodeFileSystem> nodeFileSystem_;

        virtual void unpackFileNode(const PboNode* rootNode, const PboNode* childNode, const Cancel& cancel) const;
    };
}
