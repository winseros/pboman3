#include "pboheaderreader.h"
#include <QFileInfo>
#include "pboheaderio.h"
#include "pboioexception.h"

namespace pboman3 {
    PboFileHeader PboHeaderReader::readFileHeader(PboFile* file) {
        QList<QSharedPointer<PboHeader>> headers;
        QList<QSharedPointer<PboEntry>> entries;

        const PboHeaderIO reader(file);
        QSharedPointer<PboEntry> entry = reader.readNextEntry();

        if (!entry) {
            throw PboIoException("The file is not a valid PBO.", file->fileName());
        }

        qsizetype dataBlockEnd = 0;
        if (entry->isSignature()) {
            QSharedPointer<PboHeader> header = reader.readNextHeader();
            while (header && !header->isBoundary()) {
                headers.append(header);
                header = reader.readNextHeader();
            }
            if (!header) {
                throw PboIoException("The file headers are corrupted.", file->fileName());
            }
        } else if (entry->isContent()) {
            entries.append(entry);
            dataBlockEnd += entry->dataSize();
        } else {
            throw PboIoException("The file first entry is corrupted.", file->fileName());
        }

        entry = reader.readNextEntry();
        while (entry && !entry->isBoundary()) {
            entries.append(entry);
            dataBlockEnd += entry->dataSize();
            entry = reader.readNextEntry();
        }
        if (!entry || !entry->isBoundary()) {
            throw PboIoException("The file entries list is corrupted.", file->fileName());
        }


        const qsizetype dataBlockStart = file->pos();
        dataBlockEnd += dataBlockStart;

        QByteArray signature;
        const bool seek = file->seek(dataBlockEnd + 1); //don`t forget a single 0-byte between the data end and sig start
        assert(seek);

        if (!file->atEnd()) {
            constexpr int sha1Size = 20;
            signature.resize(sha1Size);
            const qint64 read = file->read(signature.data(), signature.size());
            if (read != signature.size())
                signature.truncate(0);
        }

        return PboFileHeader{headers, entries, dataBlockStart, signature};
    }
}
