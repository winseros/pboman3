#pragma once

#include <QDir>
#include "model/interactionparcel.h"

namespace pboman3::ui {
    using namespace model;

    class FsCollector {
    public:
        static NodeDescriptors collectFiles(const QList<QUrl>& urls);

    private:
        static void collectDir(const QFileInfo& fi, const QDir& parent, NodeDescriptors& descriptors);

        static void collectFile(const QFileInfo& fi, const QDir& parent, NodeDescriptors& descriptors);
    };
}
