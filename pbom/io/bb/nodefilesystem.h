#pragma once

#include <QDir>
#include "model/pbonode.h"

namespace pboman3 {
    class NodeFileSystem : public QObject {
    Q_OBJECT
    public:
        NodeFileSystem(const QDir& folder);

        QString allocatePath(const PboNode* node) const;

        QString allocatePath(const PboNode* parent, const PboNode* node) const;

        QString composeAbsolutePath(const PboNode* node) const;

        QString composeRelativePath(const PboNode* node) const;

    private:
        QDir folder_;

        QDir makeFsPath(const PboNode* node) const;
    };

}
