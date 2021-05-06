#include "parcelmanager.h"
#include "io/bs/binarysource.h"
#include "io/bs/fsrawbinarysource.h"
#include "io/bs/pbobinarysource.h"

namespace pboman3 {
    PboParcel ParcelManager::packTree(const PboNode& root, const QList<PboPath>& paths) const {
        PboParcel parcel;
        parcel.reserve(paths.length() * 2);

        QList<PboNode*> files;
        files.reserve(paths.length());

        QSet<const PboNode*> dedupe;
        dedupe.reserve(paths.length() * 2);

        //the input list of paths may contain something like:
        //[folder1\file1.txt, folder1\]
        //thus referencing file1.txt twice
        //while in the resulting parcel we must not have duplicates
        //so we do the following:
        //1) handle folders first, starting from the top-level ones
        //2) handle standalone files after
        //that allows to avoid duplicates in the parcel

        QList<PboPath> sortedPaths(paths);
        std::sort(sortedPaths.begin(), sortedPaths.end(), [](const PboPath& a, const PboPath& b) {
            return a.length() < b.length();
        });

        for (const PboPath& path : paths) {
            const QPointer<PboNode>& node = root.get(path);
            if (node->nodeType() == PboNodeType::File) {
                files.append(node);
            } else {
                addNodeToParcel(parcel, node, "", dedupe);
            }
        }

        for (const PboNode* file : files) {
            addNodeToParcel(parcel, file, "", dedupe);
        }

        return parcel;
    }

    void ParcelManager::addNodeToParcel(PboParcel& parcel, const PboNode* node, const QString& parentPath,
                                        QSet<const PboNode*>& dedupe) const {
        if (node->nodeType() == PboNodeType::File) {
            if (!dedupe.contains(node)) {
                dedupe.insert(node);
                const QString nodePath = parentPath + node->title();
                if (const auto* pboSource = dynamic_cast<const PboBinarySource*>(node->binarySource.get())) {
                    const auto pboDataInfo = pboSource->getInfo();
                    parcel.append(PboParcelItem{
                        nodePath, pboSource->path(), pboDataInfo.dataOffset, pboDataInfo.dataSize,
                        pboDataInfo.originalSize
                    });
                } else {
                    parcel.append(PboParcelItem{nodePath, node->binarySource->path(), 0, -1, -1});
                }
            }
        } else {
            const QString nodePath = parentPath + node->title() + "/";
            auto it = node->begin();
            while (it != node->end()) {
                addNodeToParcel(parcel, *it, nodePath, dedupe);
                ++it;
            }
        }
    }

    void ParcelManager::unpackTree(QPointer<PboNode>& parent, const PboParcel& parcel,
                                   const ResolveConflictsFn& onConflict) const {

        TreeConflicts conflicts;
        for (const PboParcelItem& item : parcel) {
            conflicts.inspect(parent, item.path);
        }

        if (!conflicts.isEmpty())
            onConflict(conflicts);

        for (const PboParcelItem& item : parcel) {
            const TreeConflictResolution resolution = conflicts.getResolution(item.path);
            QPointer<PboNode> created = parent->addEntry(PboPath(item.path), resolution);
            if (created) {
                created->binarySource = item.originalSize > 0
                                            ? QSharedPointer<BinarySource>(
                                                new PboBinarySource(item.file, PboDataInfo(
                                                                        item.originalSize, item.dataSize,
                                                                        item.dataOffset
                                                                    )))
                                            : QSharedPointer<BinarySource>(new FsRawBinarySource(item.file));
            }
        }
    }
}
