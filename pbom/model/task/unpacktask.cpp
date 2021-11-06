#include "unpacktask.h"
#include <QDir>
#include <QFile>
#include "io/pboheaderreader.h"
#include "io/pboioexception.h"
#include "io/bb/unpacktaskbackend.h"
#include "io/bs/pbobinarysource.h"
#include "io/pboentry.h"
#include "domain/pbonode.h"
#include "model/rootreader.h"
#include "util/log.h"

#define LOG(...) LOGGER("model/task/UnpackTask", __VA_ARGS__)

namespace pboman3 {
    UnpackTask::UnpackTask(QString pboPath, const QString& outputDir)
        : pboPath_(std::move(pboPath)),
          outputDir_(outputDir) {
    }

    void UnpackTask::execute(const Cancel& cancel) {
        LOG(info, "PBO file: ", pboPath_)
        LOG(info, "Output dir: ", outputDir_.absolutePath())

        PboFileHeader header;
        if (!tryReadPboHeader(&header))
            return;
        QDir pboDir;
        if (!tryCreatePboDir(&pboDir))
            return;

        constexpr qsizetype startProgress = 0;
        emit taskInitialized(pboPath_, startProgress, static_cast<qint32>(header.entries.count()));

        std::function onError = [this](const QString& error) {
            emit taskMessage(error);
        };

        int progress = startProgress;
        std::function onProgress = [this, &progress]() {
            progress++;
            emit taskProgress(progress);
        };

        PboNode root("root", PboNodeType::Container, nullptr);
        RootReader(&header, pboPath_).inflateRoot(&root);

        UnpackTaskBackend be(pboDir);
        be.setOnError(&onError);
        be.setOnProgress(&onProgress);

        QList<PboNode*> childNodes;
        childNodes.reserve(root.count());
        for (PboNode* node: root)
            childNodes.append(node);
        be.unpackSync(&root, childNodes, cancel);

        LOG(info, "Unpack complete")
    }

    QDebug operator<<(QDebug debug, const UnpackTask& task) {
        return debug << "UnpackTask(PboPath=" << task.pboPath_ << ", OutputDir=" << task.outputDir_ << ")";
    }

    bool UnpackTask::tryReadPboHeader(PboFileHeader* header) {
        try {
            PboFile file(pboPath_);
            file.open(QIODeviceBase::OpenModeFlag::ReadOnly);
            *header = PboHeaderReader::readFileHeader(&file);
            LOG(info, "The file header:", *header)
            return true;
        } catch (const PboIoException& ex) {
            LOG(warning, "Got error while reading the file header:", ex)
            emit taskMessage("The file is not a PBO | " + pboPath_);
            return false;
        }
    }

    bool UnpackTask::tryCreatePboDir(QDir* dir) {
        const QString fileNameWithoutExt = GetFileNameWithoutExtension(QFileInfo(pboPath_).fileName());
        const QString absPath = outputDir_.absoluteFilePath(fileNameWithoutExt);
        if (!outputDir_.exists(fileNameWithoutExt) && !outputDir_.mkdir(fileNameWithoutExt)) {
            LOG(warning, "Could not create the directory:", absPath)
            emit taskMessage("Could not create the directory | " + absPath);
        }
        LOG(info, "PBO output dir:", absPath)
        *dir = QDir(absPath);
        return true;
    }

    bool UnpackTask::tryCreateEntryDir(const QDir& pboDir, const QSharedPointer<PboEntry>& entry) {
        QDir local(pboDir);
        PboPath path(entry->fileName());
        auto it = path.begin();
        const auto last = path.end() - 1;

        while (it != last) {
            if (!local.exists(*it) && !local.mkdir(*it)) {
                LOG(warning, "Could not create the directory:", local.absolutePath())
                emit taskMessage("Could not create the directory | " + local.absolutePath());
                return false;
            }
            local.cd(*it);
            ++it;
        }

        return true;
    }
}
