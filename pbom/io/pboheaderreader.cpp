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
            throw PboIoException("The file is not a valid PBO");
        }

        if (entry->isSignature()) {
            QSharedPointer<PboHeader> header = reader.readNextHeader();
            while (header && !header->isBoundary()) {
                headers.append(header);
                header = reader.readNextHeader();
            }
            if (!header) {
                throw PboIoException("The file headers are corrupted");
            }
        } else if (entry->isContent()) {
            entries.append(entry);
        } else {
            throw PboIoException("The file first entry is corrupted");
        }

        entry = reader.readNextEntry();
        while (entry && !entry->isBoundary()) {
            entries.append(entry);
            entry = reader.readNextEntry();
        }
        if (!entry || !entry->isBoundary()) {
            throw PboIoException("The file entries list is corrupted");
        }

        return PboFileHeader{headers, entries};
    }
}
