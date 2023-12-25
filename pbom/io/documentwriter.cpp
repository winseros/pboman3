#include "documentwriter.h"
#include <QCryptographicHash>
#include <QTemporaryFile>
#include "diskaccessexception.h"
#include "pboheaderentity.h"
#include "pboheaderio.h"
#include "util/log.h"

#define LOG(...) LOGGER("io/documentwriter", __VA_ARGS__)

namespace pboman3::io {
    DocumentWriter::DocumentWriter(QString path)
        : path_(std::move(path)) {
        assert(!path_.isEmpty() && "Path must not be empty");
    }

    void DocumentWriter::write(PboDocument* document, const Cancel& cancel) {
        assert(document && "Document must not be null");

        LOG(info, "Writing the document to:", path_)
        const bool shouldBackup = QFile::exists(path_);
        const QString filePath = shouldBackup ? path_ + ".t" : path_;

        writeInternal(document, filePath, cancel);

        if (cancel()) {
            if (!shouldBackup)
                QFile::remove(filePath); //don't assert as not critical
            LOG(info, "Cancel - clean temp files and return")
            return;
        }

        bool backupMade = false;
        const QString backupPath = path_ + ".bak";
        if (shouldBackup && !cancel()) {
            LOG(info, "Back up the original file as: ", backupPath)
            if (QFile::exists(backupPath) && !QFile::remove(backupPath)) {
                LOG(warning, "Could not remove the prev backup file - throwing;", backupPath)
                throw DiskAccessException(
                    "Could not remove the file. Check you have enough permissions and the file is not locked by another process.",
                    backupPath);
            }
            if (!QFile::rename(path_, backupPath)) {
                LOG(info, "Could not replace the prev PBO file with a write copy - throwing:", path_)
                throw DiskAccessException(
                    "Could not write to the file. Check you have enough permissions and the file is not locked by another process.",
                    path_);
            }
            backupMade = QFile::rename(filePath, path_);
            if (!backupMade) {
                LOG(warning, "Could not rename file 1 to file 2 - throwing:", filePath, "|", path_)
                throw DiskAccessException("Could not rename the file. Normally this must not happen.", filePath);
            }
        }

        if (cancel()) {
            LOG(info, "Cancel - removing the written files")
            if (backupMade) {
                if (!QFile::remove(path_)) {
                    LOG(warning, "Could not remove the file - throwing", path_)
                    throw DiskAccessException("Could not remove the file. Normally this must not happen.", path_);
                }
                LOG(info, "The original file has been already renamed - renaming back")
                if (!QFile::rename(backupPath, path_)) {
                    LOG(warning, "Could not rename file 1 to file 2 - throwing:", backupPath, "|", path_)
                    throw DiskAccessException("Could not renames the file. Normally this must not happen.", backupPath);
                }
            } else {
                if (!shouldBackup && !QFile::remove(path_)) {
                    LOG(warning, "Could not remove the file - throwing", path_)
                    throw DiskAccessException("Could not remove the file. Normally this must not happen.", path_);
                }
            }
        } else {
            LOG(info, "Assigining binary sources")
            assignBinarySources(document->root());
        }
    }

    void DocumentWriter::writeInternal(PboDocument* document, const QString& path, const Cancel& cancel) {
        LOG(info, "Writing to the file:", path)

        QTemporaryFile body;
        body.setFileName(path + ".b");
        if (!body.open()) {
            LOG(warning, "Could not open the body temp file - throwing:", body.fileName())
            throw DiskAccessException("Could not create the file.", body.fileName());
        }

        LOG(info, "Writing nodes")
        QList<QSharedPointer<PboNodeEntity>> entries;
        writeNode(&body, document->root(), entries, cancel);

        if (cancel()) {
            LOG(info, "Cancel - return")
            return;
        }

        PboFile pbo(path);
        if (!pbo.open(QIODeviceBase::ReadWrite)) {
            LOG(warning, "Could not open the file - throwing:", path)
            throw DiskAccessException("Could not create the file.", path);
        }

        LOG(info, "Writing headers")
        writeHeader(&pbo, document->headers(), entries, cancel);

        if (cancel()) {
            LOG(info, "Cancel - clean temp files and return")
            pbo.close();
            pbo.remove();
            return;
        }

        const bool seek = body.seek(0);
        assert(seek);

        LOG(info, "Copy body bytes")
        copyBody(&pbo, &body, cancel);

        LOG(info, "Calc signature")
        writeSignature(&pbo, document, cancel);

        if (cancel()) {
            LOG(info, "Cancel - clean temp files")
            pbo.close();
            pbo.remove();
        }
    }

    void DocumentWriter::writeNode(QFileDevice* file, PboNode* node, QList<QSharedPointer<PboNodeEntity>>& entries,
                                   const Cancel& cancel) {
        for (PboNode* child : *node) {
            if (cancel()) {
                LOG(info, "Cancel - return")
                return;
            }

            if (child->nodeType() == PboNodeType::File) {
                const qint64 before = file->pos();

                const auto bsClose = qScopeGuard([&child] { if (child->binarySource->isOpen()) child->binarySource->close(); });
                child->binarySource->open();
                child->binarySource->writeToPbo(file, cancel);

                const qint64 after = file->pos();

                const qint32 originalSize = child->binarySource->readOriginalSize();
                const auto dataSize = static_cast<qint32>(after - before);

                QSharedPointer<PboNodeEntity> entry(new PboNodeEntity(
                    child->makePath().toString(),
                    child->binarySource->isCompressed() ? PboPackingMethod::Packed : PboPackingMethod::Uncompressed,
                    child->binarySource->readOriginalSize(),
                    0,
                    child->binarySource->readTimestamp(),
                    dataSize));
                entries.append(entry);

                PboDataInfo data{0, 0, 0, 0, 0};
                data.originalSize = originalSize;
                data.dataSize = dataSize;
                data.dataOffset = before;
                data.timestamp = child->binarySource->readTimestamp();
                data.compressed = child->binarySource->isCompressed();

                binarySources_.insert(child, data);

                emitWriteEntry();
            } else {
                writeNode(file, child, entries, cancel);
            }
        }
    }

    void DocumentWriter::writeHeader(PboFile* file, const DocumentHeaders* headers,
                                     const QList<QSharedPointer<PboNodeEntity>>& entries, const Cancel& cancel) {
        const PboHeaderIO io(file);
        io.writeEntry(PboNodeEntity::makeSignature());

        for (const DocumentHeader* header : *headers) {
            if (cancel()) {
                break;
            }
            io.writeHeader(PboHeaderEntity(header->name(), header->value()));
        }

        if (cancel()) {
            LOG(info, "Cancel - return")
            return;
        }

        io.writeHeader(PboHeaderEntity::makeBoundary());

        for (const QSharedPointer<PboNodeEntity>& entry : entries) {
            if (cancel()) {
                LOG(info, "Cancel - break")
                break;
            }
            io.writeEntry(*entry);
        }

        if (cancel()) {
            LOG(info, "Cancel - return")
            return;
        }

        io.writeEntry(PboNodeEntity::makeBoundary());

        for (PboNode* key : binarySources_.keys()) {
            PboDataInfo& existing = binarySources_[key];
            existing.dataOffset += file->pos();
        }
    }

    void DocumentWriter::copyBody(QFileDevice* pbo, QFileDevice* body, const Cancel& cancel) {
        QByteArray data;
        data.resize(1024 * 1024);

        qsizetype copiedBytes = 0;
        const qsizetype totalBytes = body->size();

        qint64 read = body->read(data.data(), data.size());
        while (read > 0) {
            pbo->write(data.data(), read);

            copiedBytes += read;
            emitCopyBytes(copiedBytes, totalBytes);

            if (cancel()) {
                LOG(info, "Cancel - return")
                return;
            }
            read = body->read(data.data(), data.size());
        }
    }

    void DocumentWriter::writeSignature(QFileDevice* pbo, PboDocument* document, const Cancel& cancel) {
        const bool seek = pbo->seek(0);
        assert(seek);

        QCryptographicHash sha1(QCryptographicHash::Sha1);

        qsizetype processed = 0;
        const qsizetype total = pbo->size();

        constexpr qint64 bufferSize = 1024;
        char buffer[bufferSize];
        qint64 read;

        while (!cancel() && (read = pbo->read(buffer, bufferSize)) > 0) {
            sha1.addData(buffer, read);
            processed += read;
            emitCalcHash(processed, total);
        }

        if (cancel()) {
            LOG(info, "Cancel - return")
            return;
        }

        document->setSignature(sha1.result());

        pbo->write(QByteArray(1, 0));
        pbo->write(document->signature(), document->signature().count());
    }

    void DocumentWriter::assignBinarySources(PboNode* node) {
        for (PboNode* child : *node) {
            if (child->nodeType() == PboNodeType::File) {
                const PboDataInfo& existing = binarySources_.take(child);
                child->binarySource = QSharedPointer<BinarySource>(new PboBinarySource(path_, existing));
            } else {
                assignBinarySources(child);
            }
        }
    }

    void DocumentWriter::emitWriteEntry() {
        const WriteEntryEvent evt;
        emit progress(&evt);
    }

    void DocumentWriter::emitCopyBytes(qsizetype copied, qsizetype total) {
        const CopyBytesEvent evt(copied, total);
        emit progress(&evt);
    }

    void DocumentWriter::emitCalcHash(qsizetype processed, qsizetype total) {
        const CalcHashEvent evt(processed, total);
        emit progress(&evt);
    }
}
