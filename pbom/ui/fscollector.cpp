#include "fscollector.h"
#include "model/interactionparcel.h"

namespace pboman3 {
    NodeDescriptors FsCollector::collectFiles(const QList<QUrl>& urls) {
        NodeDescriptors result;
        result.reserve(100);
        for (const QUrl& url : urls) {
            QFileInfo fi(url.toLocalFile());
            if (fi.isFile()) {
                collectFile(fi, fi.dir(), result);
            } else if (fi.isDir()) {
                collectDir(fi, fi.dir(), result);
            }
        }
        std::sort(result.begin(), result.end(), [](const NodeDescriptor& f1, const NodeDescriptor& f2) {
            return f1.path() < f2.path();
        });
        return result;
    }

    void FsCollector::collectDir(const QFileInfo& fi, const QDir& parent, NodeDescriptors& descriptors) {
        const QDir d(fi.filePath() + QDir::separator());
        const QFileInfoList entries = d.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
        for (const QFileInfo& entry : entries) {
            if (entry.isFile())
                collectFile(entry, parent, descriptors);
            else if (entry.isDir())
                collectDir(entry, parent, descriptors);
        }
    }

    void FsCollector::collectFile(const QFileInfo& fi, const QDir& parent, NodeDescriptors& descriptors) {
        if (!fi.isShortcut() && !fi.isSymbolicLink()) {
            const QString fsPath = fi.canonicalFilePath();

            const QString pboPath = parent.relativeFilePath(fi.canonicalFilePath());
            descriptors.append(NodeDescriptor(QSharedPointer<BinarySource>(new FsRawBinarySource(fsPath)), pboPath));
        }
    }
}
