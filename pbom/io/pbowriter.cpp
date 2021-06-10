#include "pbowriter.h"
#include <QTemporaryFile>
#include <QCryptographicHash>
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
            throw PboIoException("Could not create the file: " + body.fileName());

        QList<PboEntry> entries;
        writeNode(&body, root_, entries, cancel);

        if (cancel()) {
            return;
        }

        PboFile pbo(path_);
        if (!pbo.open(QIODeviceBase::ReadWrite))
            throw PboIoException("Could not create the file: " + path_);

        writeHeader(&pbo, entries, cancel);

        if (cancel()) {
            pbo.close();
            pbo.remove();
            return;
        }

        body.seek(0);
        copyBody(&pbo, &body, cancel);

        writeSignature(&pbo);

        body.close();
        pbo.close();
    }

    void PboWriter::cleanBinarySources() const {
        cleanBinarySources(root_);
    }

    void PboWriter::reassignBinarySources(const QString& path) {
        reassignBinarySources(root_, path);
    }

    void PboWriter::writeNode(QFileDevice* file, PboNode* node, QList<PboEntry>& entries, const Cancel& cancel) {
        for (PboNode* child : *node) {
            if (child->nodeType() == PboNodeType::File) {
                const qint64 before = file->pos();
                child->binarySource->writeToPbo(file, cancel);
                const qint64 after = file->pos();

                const qint32 originalSize = child->binarySource->readOriginalSize();
                const qint32 dataSize = static_cast<qint32>(after - before);

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

            } else {
                writeNode(file, child, entries, cancel);
            }
        }
    }

    void PboWriter::writeHeader(PboFile* file, const QList<PboEntry>& entries, const Cancel& cancel) {
        const PboHeaderIO io(file);
        io.writeEntry(PboEntry::makeSignature());

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

        qint64 read = body->read(data.data(), data.size());
        while (read > 0) {
            pbo->write(data.data(), read);
            if (cancel())
                return;
            read = body->read(data.data(), data.size());
        }
    }

    void PboWriter::writeSignature(QFileDevice* pbo) const {
        pbo->seek(0);
        QCryptographicHash sha1(QCryptographicHash::Sha1);
        assert(sha1.addData(pbo));

        const QByteArray sha1Bytes = sha1.result();

        pbo->write(QByteArray(1, 0));
        pbo->write(sha1Bytes, sha1Bytes.count());

        if (signature_) {
            signature_->append(sha1Bytes);
        }
    }

    void PboWriter::cleanBinarySources(PboNode* node) const {
        for (PboNode* child : *node) {
            if (child->nodeType() == PboNodeType::File) {
                child->binarySource.clear();
            } else {
                cleanBinarySources(child);
            }
        }
    }

    void PboWriter::reassignBinarySources(PboNode* node, const QString& path) {
        for (PboNode* child : *node) {
            if (child->nodeType() == PboNodeType::File) {
                const PboDataInfo& existing = binarySources_.take(child);
                child->binarySource = QSharedPointer<BinarySource>(new PboBinarySource(path, existing));
            } else {
                reassignBinarySources(child, path);
            }
        }
    }
}
