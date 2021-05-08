#pragma once

#include <QDir>
#include "model/interactionparcel.h"

namespace pboman3 {
    class FsCollector {
    public:
        NodeDescriptors collectFiles(const QList<QUrl>& urls);

    private:
        void collectDir(const QFileInfo& fi, const QDir& parent, NodeDescriptors& descriptors);

        void collectFile(const QFileInfo& fi, const QDir& parent, NodeDescriptors& descriptors);
    };
}
