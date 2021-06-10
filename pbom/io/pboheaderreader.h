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
    };

    class PboHeaderReader {
    public:
        static PboFileHeader readFileHeader(PboFile* file);
    };
}
