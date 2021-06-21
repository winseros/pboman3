#pragma once

#include "pbofile.h"
#include "model/pboentry.h"
#include "model/pboheader.h"

namespace pboman3 {
    struct PboFileHeader {
        QList<QSharedPointer<PboHeader>> headers;
        QList<QSharedPointer<PboEntry>> entries;
        qsizetype dataBlockStart;
        QByteArray signature;

        friend QDebug operator<<(QDebug debug, const PboFileHeader& header) {
            return debug << "PboFileHeader(Headers=" << header.headers.length() << ", Entries=" << header.entries.
                length() << ", DataBlockStart=" << header.dataBlockStart << ", Signature=" << header.signature.length() << ")";
        }
    };

    class PboHeaderReader {
    public:
        static PboFileHeader readFileHeader(PboFile* file);
    };
}
