#include "documentwriter.h"
#include <QCryptographicHash>
#include <QTemporaryFile>
#include "pboheader.h"
#include "pboheaderio.h"
#include "pboioexception.h"

namespace pboman3::io {
    DocumentWriter::DocumentWriter(QString path)
        : path_(std::move(path)) {
        assert(!path_.isEmpty() && "Path must not be empty");
    }

    void DocumentWriter::write(PboDocument* document, const Cancel& cancel) {
        assert(document && "Document must not be null");

        const QString filePath = QFile::exists(path_) ? path_ + ".t" : path_;
        writeInternal(document, filePath, cancel);

        suspendBinarySources(document->root());

        if (filePath != path_ && !cancel()) {
            const QString backupPath = path_ + ".bak";
            //LOG(info, "Cleaning up the temporary files")
            if (QFile::exists(backupPath) && !QFile::remove(backupPath)) {
                //LOG(info, "Could not remove the prev backup file - throwing;", backupPath)
                resumeBinarySources(document->root());
                throw PboIoException(
                    "Could not remove the file. Check you have enough permissions and the file is not locked by another process.",
                    backupPath);
            }
            if (!QFile::rename(path_, backupPath)) {
                //LOG(info, "Could not replace the prev PBO file with a write copy - throwing;", loadedPath_)
                resumeBinarySources(document->root());
                throw PboIoException(
                    "Could not write to the file. Check you have enough permissions and the file is not locked by another process.",
                    path_);
            }
            const bool renamed = QFile::rename(filePath, path_);
            assert(renamed);
        }

        if (cancel()) {
            resumeBinarySources(document->root());
        } else {
            assignBinarySources(document->root());
        }
    }

    void DocumentWriter::writeInternal(PboDocument* document, const QString& path, const Cancel& cancel) {
        QTemporaryFile body;
        body.setFileName(path + ".b");
        if (!body.open())
            throw PboIoException("Could not create the file.", body.fileName());

        QList<QSharedPointer<PboEntry>> entries;
        writeNode(&body, document->root(), entries, cancel);

        if (cancel())
            return;

        PboFile pbo(path);
        if (!pbo.open(QIODeviceBase::ReadWrite))
            throw PboIoException("Could not create the file.", path);

        writeHeader(&pbo, document->headers(), entries, cancel);

        if (cancel()) {
            pbo.close();
            pbo.remove();
            return;
        }

        const bool seek = body.seek(0);
        assert(seek);

        copyBody(&pbo, &body, cancel);

        writeSignature(&pbo, document, cancel);

        if (cancel()) {
            pbo.close();
            pbo.remove();
        }
    }

    void DocumentWriter::writeNode(QFileDevice* file, DocumentNode* node, QList<QSharedPointer<PboEntry>>& entries,
                                   const Cancel& cancel) {
        for (DocumentNode* child : *node) {
            if (cancel())
                return;

            if (child->nodeType() == PboNodeType::File) {
                const qint64 before = file->pos();
                child->binarySource->writeToPbo(file, cancel);
                const qint64 after = file->pos();

                const qint32 originalSize = child->binarySource->readOriginalSize();
                const auto dataSize = static_cast<qint32>(after - before);

                QSharedPointer<PboEntry> entry(new PboEntry(
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
                                     const QList<QSharedPointer<PboEntry>>& entries, const Cancel& cancel) {
        const PboHeaderIO io(file);
        io.writeEntry(PboEntry::makeSignature());

        for (const DocumentHeader* header : *headers) {
            if (cancel()) {
                break;
            }
            io.writeHeader(PboHeader(header->name(), header->value()));
        }

        if (cancel()) {
            return;
        }

        io.writeHeader(PboHeader::makeBoundary());

        for (const QSharedPointer<PboEntry>& entry : entries) {
            if (cancel()) {
                break;
            }
            io.writeEntry(*entry);
        }

        if (cancel()) {
            return;
        }

        io.writeEntry(PboEntry::makeBoundary());

        for (DocumentNode* key : binarySources_.keys()) {
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

            if (cancel())
                return;
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

        if (cancel())
            return;

        document->setSignature(sha1.result());

        pbo->write(QByteArray(1, 0));
        pbo->write(document->signature(), document->signature().count());
    }

    void DocumentWriter::suspendBinarySources(DocumentNode* node) const {
        for (DocumentNode* child : *node) {
            if (child->nodeType() == PboNodeType::File) {
                child->binarySource->close();
            }
            else {
                suspendBinarySources(child);
            }
        }
    }

    void DocumentWriter::resumeBinarySources(DocumentNode* node) const {
        for (DocumentNode* child : *node) {
            if (child->nodeType() == PboNodeType::File) {
                child->binarySource->open();
            }
            else {
                resumeBinarySources(child);
            }
        }
    }

    void DocumentWriter::assignBinarySources(DocumentNode* node) {
        for (DocumentNode* child : *node) {
            if (child->nodeType() == PboNodeType::File) {
                const PboDataInfo& existing = binarySources_.take(child);
                child->binarySource = QSharedPointer<BinarySource>(new PboBinarySource(path_, existing));
                child->binarySource->open();
            }
            else {
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
