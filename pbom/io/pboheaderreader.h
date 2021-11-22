#pragma once

#include "pbofile.h"
#include "io/pbonodeentity.h"
#include "io/pboheaderentity.h"
#include <QDebug>

namespace pboman3::io {
    struct PboFileHeader {
        QList<QSharedPointer<PboHeaderEntity>> headers;
        QList<QSharedPointer<PboNodeEntity>> entries;
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
