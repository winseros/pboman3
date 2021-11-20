#include "pboheaderreader.h"
#include <QFileInfo>
#include "pbofileformatexception.h"
#include "pboheaderio.h"

namespace pboman3::io {
    PboFileHeader PboHeaderReader::readFileHeader(PboFile* file) {
        QList<QSharedPointer<PboHeaderEntity>> headers;
        QList<QSharedPointer<PboNodeEntity>> entries;

        const PboHeaderIO reader(file);
        QSharedPointer<PboNodeEntity> entry = reader.readNextEntry();

        if (!entry) {
            throw PboFileFormatException("The file is not a valid PBO.");
        }

        qsizetype dataBlockEnd = 0;
        if (entry->isSignature()) {
            QSharedPointer<PboHeaderEntity> header = reader.readNextHeader();
            while (header && !header->isBoundary()) {
                headers.append(header);
                header = reader.readNextHeader();
            }
            if (!header) {
                throw PboFileFormatException("The file headers are corrupted.");
            }
        } else if (entry->isContent()) {
            entries.append(entry);
            dataBlockEnd += entry->dataSize();
        } else {
            throw PboFileFormatException("The file first entry is corrupted.");
        }

        entry = reader.readNextEntry();
        while (entry && !entry->isBoundary()) {
            entries.append(entry);
            dataBlockEnd += entry->dataSize();
            entry = reader.readNextEntry();
        }
        if (!entry || !entry->isBoundary()) {
            throw PboFileFormatException("The file entries list is corrupted.");
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
