#pragma once

#include "pbofile.h"
#include "model/pboentry.h"
#include "model/pboheader.h"

namespace pboman3 {
    using namespace std;

    class PboHeaderIO {
    public:
        explicit PboHeaderIO(PboFile* file);

        QSharedPointer<PboEntry> readNextEntry() const;

        QSharedPointer<PboHeader> readNextHeader() const;

        void writeEntry(const PboEntry& entry) const;

        void writeHeader(const PboHeader& header) const;

    private:
        PboFile* file_;
    };
}
