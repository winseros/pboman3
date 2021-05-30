#include "pbowriter.h"
#include <QTemporaryFile>
#include "pbofile.h"
#include "pboheaderio.h"
#include "pboioexception.h"

namespace pboman3 {
    PboWriter::PboWriter()
        : root_(nullptr) {
    }

    PboWriter& PboWriter::usePath(QString path) {
        path_ = std::move(path);
        return *this;
    }

    PboWriter& PboWriter::addHeader(PboHeader* header) {
        headers_.append(header);
        return *this;
    }

    PboWriter& PboWriter::useRoot(PboNode* root) {
        root_ = root;
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
        if (!pbo.open(QIODeviceBase::WriteOnly))
            throw PboIoException("Could not create the file: " + path_);

        writeHeader(&pbo, entries, cancel);

        if (cancel()) {
            pbo.close();
            pbo.remove();
            return;
        }

        body.seek(0);
        copyBody(&pbo, &body, cancel);

        pbo.close();
    }

    void PboWriter::writeNode(QFileDevice* file, PboNode* node, QList<PboEntry>& entries, const Cancel& cancel) {
        for (const QSharedPointer<PboNode>& child : *node) {
            if (child->nodeType() == PboNodeType::File) {
                const qint64 before = file->pos();
                child->binarySource->writeToPbo(file, cancel);
                const qint64 after = file->pos();

                PboEntry entry(
                    child->makePath().toString(),
                    child->binarySource->isCompressed() ? PboPackingMethod::Packed : PboPackingMethod::Uncompressed,
                    child->binarySource->readOriginalSize(),
                    0,
                    child->binarySource->readTimestamp(),
                    after - before);
                entries.append(entry);
            } else {
                writeNode(file, child.get(), entries, cancel);
            }
        }
    }

    void PboWriter::writeHeader(PboFile* file, const QList<PboEntry>& entries, const Cancel& cancel) {
        const PboHeaderIO io(file);
        io.writeEntry(PboEntry::makeSignature());

        for (const PboHeader* header : headers_) {
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
    }

    void PboWriter::copyBody(QFileDevice* pbo, QFileDevice* body, const Cancel& cancel) {
        QByteArray data;
        data.resize(1024 * 1024);

        qint64 read = body->read(data.data(), data.size());
        while (read > 0) {
            qDebug() << body->pos() << " || " << read;
            pbo->write(data.data(), read);
            if (cancel())
                return;
            read = body->read(data.data(), data.size());
        }
    }
}
