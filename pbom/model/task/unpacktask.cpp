#include "unpacktask.h"
#include <QDir>
#include <QFile>
#include "io/pboheaderreader.h"
#include "io/pboioexception.h"
#include "io/bs/pbobinarysource.h"
#include "model/pboentry.h"
#include "util/log.h"

#define LOG(...) LOGGER("model/task/UnpackTask", __VA_ARGS__)

namespace pboman3 {
    UnpackTask::UnpackTask(QString pboPath, const QString& targetPath)
        : pboPath_(std::move(pboPath)),
          targetDir_(targetPath) {
    }

    void UnpackTask::execute(const Cancel& cancel) {
        LOG(info, "PBO file: ", pboPath_)
        LOG(info, "Target dir: ", targetDir_.absolutePath())

        PboFileHeader header;
        if (!tryReadPboHeader(&header))
            return;
        QDir pboDir;
        if (!tryCreatePboDir(&pboDir))
            return;

        constexpr qsizetype startProgress = 0;
        emit taskInitialized(pboPath_, startProgress, static_cast<qint32>(header.entries.count()));

        qsizetype entryDataOffset = header.dataBlockStart;
        qint32 progress = startProgress;
        for (const QSharedPointer<PboEntry>& entry : header.entries) {
            if (cancel()) break;

            LOG(debug, "Processing the entry:", *entry)
            PboDataInfo di{0, 0, 0, 0, 0};
            di.originalSize = entry->originalSize();
            di.dataSize = entry->dataSize();
            di.dataOffset = entryDataOffset;
            di.timestamp = entry->timestamp();
            di.compressed = entry->packingMethod() == PboPackingMethod::Packed;
            PboBinarySource bs(pboPath_, di);
            bs.open();

            if (tryCreateEntryDir(pboDir, entry)) {
                QFile entryFile(pboDir.absoluteFilePath(entry->fileName()));
                if (entryFile.open(QIODeviceBase::NewOnly | QIODeviceBase::ReadWrite)) {
                    LOG(info, "Write to the file:", entryFile.fileName())
                    bs.writeToFs(&entryFile, cancel);
                    entryFile.close();
                } else {
                    LOG(info, "Could not create the file:", entryFile.fileName())
                    emit taskMessage("Could not create file | " + entryFile.fileName());
                }
            }

            entryDataOffset += di.dataSize;
            progress++;
            emit taskProgress(progress);
        }
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
        const QString absPath = targetDir_.absoluteFilePath(fileNameWithoutExt);
        if (!targetDir_.exists(fileNameWithoutExt) && !targetDir_.mkdir(fileNameWithoutExt)) {
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
