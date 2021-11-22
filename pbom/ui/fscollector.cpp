#include "fscollector.h"
#include "model/interactionparcel.h"
#include "util/log.h"

#define LOG(...) LOGGER("ui/FsCollector", __VA_ARGS__)

namespace pboman3::ui {
    NodeDescriptors FsCollector::collectFiles(const QList<QUrl>& urls) {
        LOG(info, "Collecting the files at:", urls)

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

        LOG(info, "Collected files:", result.size())

        return result;
    }

    void FsCollector::collectDir(const QFileInfo& fi, const QDir& parent, NodeDescriptors& descriptors) {
        LOG(debug, "Collecting the dir:", fi)

        const QDir d(fi.filePath() + QDir::separator());
        const QFileInfoList entries = d.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
        for (const QFileInfo& entry : entries) {
            if (!entry.isSymLink()) {
                if (entry.isFile())
                    collectFile(entry, parent, descriptors);
                else if (entry.isDir())
                    collectDir(entry, parent, descriptors);
            }
        }
    }

    void FsCollector::collectFile(const QFileInfo& fi, const QDir& parent, NodeDescriptors& descriptors) {
        LOG(debug, "Collecting the file:", fi)

        if (!fi.isShortcut() && !fi.isSymbolicLink()) {
            QString fsPath = fi.canonicalFilePath();

            const QString pboPath = parent.relativeFilePath(fi.canonicalFilePath());

            const auto bs =  QSharedPointer<BinarySource>(new FsRawBinarySource(std::move(fsPath)));
            bs->open();
                                              
            descriptors.append(NodeDescriptor(bs, PboPath(pboPath)));

            LOG(debug, "File collected")
        }
    }
}
