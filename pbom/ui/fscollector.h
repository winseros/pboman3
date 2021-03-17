#pragma once

#include <QDir>
#include "model/filesystemfiles.h"

namespace pboman3 {
    class FsCollector {
    public:
        FilesystemFiles collectFiles(const QList<QUrl>& urls);

    private:
        void collectDir(const QFileInfo& fi, const QDir& parent, FilesystemFiles& collection);

        void collectFile(const QFileInfo& fi, const QDir& parent, FilesystemFiles& collection);
    };
}
