#pragma once

#include "pbofile.h"
#include "model/pboentry.h"
#include "model/pboheader.h"
#include "model/pbonode.h"

namespace pboman3 {
    struct PboFileHeader {
        QList<QSharedPointer<PboHeader>> headers;
        QList<QSharedPointer<PboEntry>> entries;
    };

    class PboHeaderReader {
    public:
        static PboFileHeader readFileHeader(PboFile* file);
    };
}
