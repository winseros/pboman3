#include "fscollector.h"
#include <QUrl>

namespace pboman3 {
    FilesystemFiles FsCollector::collectFiles(const QList<QUrl>& urls) {
        FilesystemFiles result;
        result.reserve(100);
        for (const QUrl& url : urls) {
            QFileInfo fi(url.toLocalFile());
            if (fi.isFile()) {
                collectFile(fi, fi.dir(), result);
            } else if (fi.isDir()) {
                collectDir(fi, fi.dir(), result);
            }
        }
        std::sort(result.begin(), result.end(), [](const FilesystemFile& f1, const FilesystemFile& f2) {
            return f1 < f2;
        });
        return result;
    }

    void FsCollector::collectDir(const QFileInfo& fi, const QDir& parent, FilesystemFiles& collection) {
        const QDir d(fi.filePath() + QDir::separator());
        const QFileInfoList entries = d.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
        for (const QFileInfo& entry : entries) {
            if (entry.isFile())
                collectFile(entry, parent, collection);
            else if (entry.isDir())
                collectDir(entry, parent, collection);
        }
    }

    void FsCollector::collectFile(const QFileInfo& fi, const QDir& parent, FilesystemFiles& collection) {
        if (!fi.isShortcut() && !fi.isSymbolicLink()) {
            const QString fsPath = fi.canonicalFilePath();
            const QString pboPath = parent.relativeFilePath(fi.canonicalFilePath());
            collection.append(FilesystemFile{fsPath, pboPath, false});
        }
    }
}
