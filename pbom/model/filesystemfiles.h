#pragma once

#include <QString>

namespace pboman3 {
    struct FilesystemFile {
        QString fsPath;

        QString pboPath;

        bool compress;

        bool operator<(const FilesystemFile& other) const;
    };

    inline bool FilesystemFile::operator<(const FilesystemFile& other) const {
        return pboPath < other.pboPath;
    }

    typedef QList<FilesystemFile> FilesystemFiles;
}
