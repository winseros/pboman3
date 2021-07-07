#pragma once

#include <QDir>
#include "model/pbonode.h"

namespace pboman3 {
    class PboNode;

    class UnpackNodes {
    public:
        void unpackTo(const QString& dir, const PboNode* rootNode, const QList<PboNode*>& childNodes, const Cancel& cancel);

    private:
        void unpackNode(const QString& dir, const PboNode* rootNode, const PboNode* childNode, const Cancel& cancel);

        void unpackFolderNode(const QString& dir, const PboNode* rootNode, const PboNode* childNode, const Cancel& cancel);

        void unpackFileNode(const QString& dir, const PboNode* rootNode, const PboNode* childNode, const Cancel& cancel);

        QString createPathForFile(const QString& dir, const PboNode* rootNode, const PboNode* childNode);
    };
}
