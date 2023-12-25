#include "fscollector.h"
#include "model/interactionparcel.h"
#include "util/log.h"

#define LOG(...) LOGGER("ui/FsCollector", __VA_ARGS__)

namespace pboman3::ui {
    QSharedPointer<NodeDescriptors> FsCollector::collectFiles(const QList<QUrl>& urls, const Cancel& cancel) {
        LOG(info, "Collecting the files at:", urls)

        QSharedPointer<NodeDescriptors> result(new NodeDescriptors);
        result->reserve(100);

        for (const QUrl& url : urls) {
            if (cancel()) 
                return nullptr;
            QFileInfo fi(url.toLocalFile());
            if (!fi.isSymLink()) {
                if (fi.isFile()) {
                    collectFile(fi, fi.dir(), result.get());
                } else if (fi.isDir()) {
                    collectDir(fi, fi.dir(), result.get(), cancel);
                }
            }
        }
        std::sort(result->begin(), result->end(), [](const NodeDescriptor& f1, const NodeDescriptor& f2) {
            return f1.path() < f2.path();
        });

        LOG(info, "Collected files:", result->size())

        return result;
    }

    void FsCollector::collectDir(const QFileInfo& fi, const QDir& parent, NodeDescriptors* descriptors, const Cancel& cancel) {
        LOG(debug, "Collecting the dir:", fi)

        const QDir d(fi.filePath() + QDir::separator());
        const QFileInfoList entries = d.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
        for (const QFileInfo& entry : entries) {
            if (cancel()) 
                return;
            if (!entry.isSymLink()) {
                if (entry.isFile())
                    collectFile(entry, parent, descriptors);
                else if (entry.isDir())
                    collectDir(entry, parent, descriptors, cancel);
            }
        }
    }

    void FsCollector::collectFile(const QFileInfo& fi, const QDir& parent, NodeDescriptors* descriptors) {
        LOG(debug, "Collecting the file:", fi)

        if (!fi.isShortcut() && !fi.isSymbolicLink()) {
            QString fsPath = fi.canonicalFilePath();

            const QString pboPath = parent.relativeFilePath(fi.canonicalFilePath());

            const auto bs = QSharedPointer<BinarySource>(new FsRawBinarySource(std::move(fsPath)));
            descriptors->append(NodeDescriptor(bs, PboPath(pboPath)));

            LOG(debug, "File collected")
        }
    }
}
