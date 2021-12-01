#pragma once

#include <QDir>
#include "model/interactionparcel.h"

namespace pboman3::ui {
    using namespace model;

    class FsCollector {
    public:
        static QSharedPointer<NodeDescriptors> collectFiles(const QList<QUrl>& urls, const Cancel& cancel);

    private:
        static void collectDir(const QFileInfo& fi, const QDir& parent, NodeDescriptors* descriptors, const Cancel& cancel);

        static void collectFile(const QFileInfo& fi, const QDir& parent, NodeDescriptors* descriptors);
    };
}
