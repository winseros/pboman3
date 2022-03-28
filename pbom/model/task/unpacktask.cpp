#include "unpacktask.h"
#include <QDir>
#include <QFile>

#include "extractconfiguration.h"
#include "packoptions.h"
#include "io/bb/unpacktaskbackend.h"
#include "io/bs/pbobinarysource.h"
#include "io/pbonodeentity.h"
#include "domain/pbonode.h"
#include "domain/func.h"
#include "io/diskaccessexception.h"
#include "io/documentreader.h"
#include "io/pbofileformatexception.h"
#include "util/log.h"

#define LOG(...) LOGGER("model/task/UnpackTask", __VA_ARGS__)

namespace pboman3::model::task {
    using namespace io;

    UnpackTask::UnpackTask(QString pboPath, const QString& outputDir)
        : pboPath_(std::move(pboPath)),
          outputDir_(outputDir) {
    }

    void UnpackTask::execute(const Cancel& cancel) {
        LOG(info, "PBO file: ", pboPath_)
        LOG(info, "Output dir: ", outputDir_.absolutePath())

        QSharedPointer<PboDocument> document;
        if (!tryReadPboHeader(&document))
            return;
        QDir pboDir;
        if (!tryCreatePboDir(&pboDir))
            return;

        constexpr qsizetype startProgress = 0;
        qint32 endProgress = 0;
        CountFilesInTree(*document->root(), endProgress);
        emit taskInitialized(pboPath_, startProgress, endProgress);

        std::function onError = [this](const QString& error) {
            emit taskMessage(error);
        };

        int progress = startProgress;
        std::function onProgress = [this, &progress]() {
            progress++;
            emit taskProgress(progress);
        };

        UnpackTaskBackend be(pboDir);
        be.setOnError(&onError);
        be.setOnProgress(&onProgress);

        QList<PboNode*> childNodes;
        childNodes.reserve(document->root()->count());
        for (PboNode* node : *document->root())
            childNodes.append(node);
        be.unpackSync(document->root(), childNodes, cancel);

        extractPboConfig(*document, pboDir);

        LOG(info, "Unpack complete")
    }

    QDebug operator<<(QDebug debug, const UnpackTask& task) {
        return debug << "UnpackTask(PboPath=" << task.pboPath_ << ", OutputDir=" << task.outputDir_ << ")";
    }

    bool UnpackTask::tryReadPboHeader(QSharedPointer<PboDocument>* document) {
        try {
            const DocumentReader reader(pboPath_);
            *document = reader.read();
            LOG(debug, "The document:", *document)
            return true;
        } catch (const DiskAccessException& ex) {
            LOG(warning, "Got error while opening the file:", ex)
            emit taskMessage("Can not read the file | " + pboPath_);
            return false;
        } catch (const PboFileFormatException& ex) {
            LOG(warning, "Got error while reading the file document:", ex)
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

    bool UnpackTask::tryCreateEntryDir(const QDir& pboDir, const QSharedPointer<PboNode>& entry) {
        QDir local(pboDir);
        PboPath path = entry->makePath();
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

    void UnpackTask::extractPboConfig(const PboDocument& document, const QDir& dir) {
        const PackOptions options = ExtractConfiguration::extractFrom(document);
        LOG(info, "Extracted the PBO pack config, Options=", options)
        ExtractConfiguration::saveTo(options, dir);
    }
}
