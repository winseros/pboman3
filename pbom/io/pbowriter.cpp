#include "pbowriter.h"
#include <QTemporaryFile>
#include <QCryptographicHash>
#include <QDir>

#include "pbofile.h"
#include "pboheaderio.h"
#include "pboioexception.h"
#include "bs/pbobinarysource.h"

namespace pboman3 {
    PboWriter::PboWriter()
        : root_(nullptr),
          headers_(nullptr),
          signature_(nullptr) {
    }

    PboWriter& PboWriter::usePath(QString path) {
        path_ = std::move(path);
        return *this;
    }

    PboWriter& PboWriter::useHeaders(HeadersModel* headers) {
        headers_ = headers;
        return *this;
    }

    PboWriter& PboWriter::useRoot(PboNode* root) {
        root_ = root;
        return *this;
    }

    PboWriter& PboWriter::copySignatureTo(QByteArray* signature) {
        signature_ = signature;
        return *this;
    }

    void PboWriter::write(const Cancel& cancel) {
        assert(!path_.isEmpty() && "Path must not be empty");
        assert(root_ && "Root must not be null");

        QTemporaryFile body;
        body.setFileName(path_ + ".b");
        if (!body.open())
            throw PboIoException("Could not create the file.", body.fileName());

        QList<PboEntry> entries;
        writeNode(&body, root_, entries, cancel);

        if (cancel())
            return;

        PboFile pbo(path_);
        if (!pbo.open(QIODeviceBase::ReadWrite))
            throw PboIoException("Could not create the file.", path_);

        writeHeader(&pbo, entries, cancel);

        if (cancel()) {
            pbo.close();
            pbo.remove();
            return;
        }

        const bool seek = body.seek(0);
        assert(seek);

        copyBody(&pbo, &body, cancel);

        writeSignature(&pbo, cancel);

        body.close();
        pbo.close();

        if (cancel()) {
            pbo.remove();
        }
    }

    void PboWriter::suspendBinarySources() const {
        suspendBinarySources(root_);
    }

    void PboWriter::resumeBinarySources() const {
        resumeBinarySources(root_);
    }

    void PboWriter::assignBinarySources(const QString& path) {
        assignBinarySources(root_, path);
    }

    void PboWriter::writeNode(QFileDevice* file, PboNode* node, QList<PboEntry>& entries, const Cancel& cancel) {
        for (PboNode* child : *node) {
            if (cancel())
                return;

            if (child->nodeType() == PboNodeType::File) {
                const qint64 before = file->pos();
                child->binarySource->writeToPbo(file, cancel);
                const qint64 after = file->pos();

                const qint32 originalSize = child->binarySource->readOriginalSize();
                const auto dataSize = static_cast<qint32>(after - before);

                PboEntry entry(
                    child->makePath().toString(),
                    child->binarySource->isCompressed() ? PboPackingMethod::Packed : PboPackingMethod::Uncompressed,
                    child->binarySource->readOriginalSize(),
                    0,
                    child->binarySource->readTimestamp(),
                    dataSize);
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

    void PboWriter::writeHeader(PboFile* file, const QList<PboEntry>& entries, const Cancel& cancel) {
        const PboHeaderIO io(file);
        io.writeEntry(PboEntry::makeSignature());

        bool prefixFound = false;
        for (const QSharedPointer<PboHeader>& header : *headers_) {
            if (cancel()) {
                break;
            }
            if (header->name == "prefix") {
                prefixFound = true;
                break;
            }
        }        
        if (!prefixFound) {
            io.writeHeader(*new PboHeader("prefix", file->fileName().remove(".pbo")));
        }

        for (const QSharedPointer<PboHeader>& header : *headers_) {
            if (cancel()) {
                break;
            }
            io.writeHeader(*header);
        }

        if (cancel()) {
            return;
        }

        io.writeHeader(PboHeader::makeBoundary());

        for (const PboEntry& entry : entries) {
            if (cancel()) {
                break;
            }
            io.writeEntry(entry);
        }

        if (cancel()) {
            return;
        }

        io.writeEntry(PboEntry::makeBoundary());

        for (PboNode* key : binarySources_.keys()) {
            PboDataInfo& existing = binarySources_[key];
            existing.dataOffset += file->pos();
        }
    }

    void PboWriter::copyBody(QFileDevice* pbo, QFileDevice* body, const Cancel& cancel) {
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

    void PboWriter::writeSignature(QFileDevice* pbo, const Cancel& cancel) {
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

        const QByteArray sha1Bytes = sha1.result();

        pbo->write(QByteArray(1, 0));
        pbo->write(sha1Bytes, sha1Bytes.count());

        if (signature_) {
            signature_->append(sha1Bytes);
        }
    }

    void PboWriter::suspendBinarySources(PboNode* node) const {
        for (PboNode* child : *node) {
            if (child->nodeType() == PboNodeType::File) {
                child->binarySource->close();
            } else {
                suspendBinarySources(child);
            }
        }
    }

    void PboWriter::resumeBinarySources(PboNode* node) const {
        for (PboNode* child : *node) {
            if (child->nodeType() == PboNodeType::File) {
                child->binarySource->open();
            } else {
                resumeBinarySources(child);
            }
        }
    }

    void PboWriter::assignBinarySources(PboNode* node, const QString& path) {
        for (PboNode* child : *node) {
            if (child->nodeType() == PboNodeType::File) {
                const PboDataInfo& existing = binarySources_.take(child);
                child->binarySource = QSharedPointer<BinarySource>(new PboBinarySource(path, existing));
                child->binarySource->open();
            } else {
                assignBinarySources(child, path);
            }
        }
    }

    void PboWriter::emitWriteEntry() {
        const WriteEntryEvent evt;
        emit progress(&evt);
    }

    void PboWriter::emitCopyBytes(qsizetype copied, qsizetype total) {
        const CopyBytesEvent evt(copied, total);
        emit progress(&evt);
    }

    void PboWriter::emitCalcHash(qsizetype processed, qsizetype total) {
        const CalcHashEvent evt(processed, total);
        emit progress(&evt);
    }
}
