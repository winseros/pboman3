#pragma once

#include <QDir>
#include "model/pbonode.h"

namespace pboman3 {
    class UnpackNodes {
    public:
        static void unpackTo(const QString& dir, const PboNode* rootNode, const QList<PboNode*>& childNodes, const Cancel& cancel);

    private:
        static void unpackNode(const QString& dir, const PboNode* rootNode, const PboNode* childNode, const Cancel& cancel);

        static void unpackFolderNode(const QString& dir, const PboNode* rootNode, const PboNode* childNode, const Cancel& cancel);

        static void unpackFileNode(const QString& dir, const PboNode* rootNode, const PboNode* childNode, const Cancel& cancel);

        static QString createPathForFile(const QString& dir, const PboNode* rootNode, const PboNode* childNode);
    };
}
