#include "packtask.h"

#include "io/diskaccessexception.h"
#include "io/documentwriter.h"
#include "util/log.h"

#define LOG(...) LOGGER("model/task/PackTask", __VA_ARGS__)

namespace pboman3::model {
    using namespace io;

    PackTask::PackTask(QString folder, QString outputDir)
        : folder_(std::move(folder)),
          outputDir_(std::move(outputDir)) {
    }

    void PackTask::execute(const Cancel& cancel) {
        LOG(info, "Folder file: ", folder_)
        LOG(info, "Output dir: ", outputDir_)

        const QFileInfo fi(folder_);

        const QString pboFile = QDir(outputDir_).filePath(fi.fileName()).append(".pbo");
        LOG(info, "The pbo file name:", pboFile)
        if (QFileInfo(pboFile).exists()) {
            LOG(info, "The pbo file already exists")
            emit taskMessage("Failure | File already exists | " + pboFile);
            return;
        }

        emit taskThinking(fi.absoluteFilePath());

        PboDocument document("root");
        const qint32 filesCount = collectDir(fi, fi.dir(), *document.root(), cancel);

        if (cancel())
            return;

        if (filesCount == 0) {
            LOG(info, "The Folder was empty")
            emit taskMessage("Failure | The folder is empty | " + fi.absolutePath());
            return;
        }

        DocumentWriter writer(pboFile);

        //it is tricky to display real PBO pack progress as the process consists of four independent steps.
        //1. Scan the source folder and grab files. It might take time we can't estimate at all. So just show "indeterminate" progress indicator.
        //2. Write the files from the folder into a "temp pbo body", optionally, having applied compression.
        //2a. Using the compression results, compose and write "pbo header" to the resulting file. This step is very fast, so don't consider it in UX.
        //3. Copy the body bytes from the "temp pbo body" to the resulting file.
        //4. Read the resulting file from the beginning and calculate its SHA1 checksum.
        //
        //All the major steps take time that is hard to estimate precisely, so here is the trick: we assign each step its own relative weight.
        //Depending on which step reports its progress, we increment the overall progress counter with the appropriate proportion.

        //Relative weights of the steps
#define WT_ENTRIES 1
#define WT_BODY 2
#define WT_SIGNATURE 7

        emit taskInitialized(fi.absoluteFilePath(), 0, filesCount * (WT_ENTRIES + WT_BODY + WT_SIGNATURE));

        qint32 progress = 0;
        connect(&writer, &DocumentWriter::progress, [this, &progress, filesCount](const DocumentWriter::ProgressEvent* evt) {
            if (dynamic_cast<const DocumentWriter::WriteEntryEvent*>(evt)) {
                //2nd step - just increment progress by 1 for each processed file
                progress++;
            } else if (const auto evt1 = dynamic_cast<const DocumentWriter::CopyBytesEvent*>(evt)) {
                //3rd step - see how many bytes copied and how it corresponds to the overall progress
                progress = filesCount * WT_ENTRIES + static_cast<qint32>(1.0 * filesCount * WT_BODY
                    / static_cast<double>(evt1->total) * static_cast<double>(evt1->copied));
            } else if (const auto evt2 = dynamic_cast<const DocumentWriter::CalcHashEvent*>(evt)) {
                //4th step - see how many bytes were processed for signature and update the overall progress
                //once all bytes processed - report 100% progress explicitly
                progress = evt2->processed == evt2->total
                               ? filesCount * (WT_ENTRIES + WT_BODY + WT_SIGNATURE)
                               : filesCount * (WT_ENTRIES + WT_BODY) + static_cast<qint32>(1.0 * filesCount *
                                   WT_SIGNATURE / static_cast<double>(evt2->total) * static_cast<double>(evt2->
                                       processed));
            }
            emit taskProgress(progress);
        });

        try {
            writer.write(&document, cancel);
            LOG(info, "Unpack complete")
        } catch (const DiskAccessException& ex) {
            LOG(warning, "Task failed with exception:", ex)
            emit taskMessage("Failure | " + ex.message() + " | " + fi.absolutePath());
        }
    }

    QDebug operator<<(QDebug debug, const PackTask& task) {
        return debug << "PackTask(Folder=" << task.folder_ << ", OutputDir=" << task.outputDir_ << ")";
    }

    qint32 PackTask::collectDir(const QFileInfo& dirEntry, const QDir& rootDir, PboNode& rootNode,
                                const Cancel& cancel) const {
        LOG(debug, "Collecting the dir:", dirEntry)

        qint32 count = 0;

        const QDir d(dirEntry.filePath() + QDir::separator());
        const QFileInfoList entries = d.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
        for (const QFileInfo& entry : entries) {
            if (cancel())
                return 0;

            if (!entry.isSymLink()) {
                if (entry.isFile())
                    count += collectFile(entry, rootDir, rootNode);
                else if (entry.isDir())
                    count += collectDir(entry, rootDir, rootNode, cancel);
            }
        }

        return count;
    }

    qint32 PackTask::collectFile(const QFileInfo& fileEntry, const QDir& rootDir, PboNode& rootNode) const {
        LOG(debug, "Collecting the file:", fileEntry)

        if (!fileEntry.isShortcut() && !fileEntry.isSymbolicLink()) {
            QString fsPath = fileEntry.canonicalFilePath();

            const QString pboPath = rootDir.relativeFilePath(fileEntry.canonicalFilePath());
            PboNode* node = rootNode.createHierarchy(PboPath(pboPath));
            node->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(std::move(fsPath)));
            node->binarySource->open();

            LOG(debug, "File collected")

            return 1;
        }
        return 0;
    }
}
