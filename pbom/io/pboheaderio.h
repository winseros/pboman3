#pragma once

#include "pbofile.h"
#include "io/pbonodeentity.h"
#include "io/pboheaderentity.h"
#include <QSharedPointer>

namespace pboman3::io {
    using namespace std;

    class PboHeaderIO {
    public:
        explicit PboHeaderIO(PboFile* file);

        QSharedPointer<PboNodeEntity> readNextEntry() const;

        QSharedPointer<PboHeaderEntity> readNextHeader() const;

        void writeEntry(const PboNodeEntity& entry) const;

        void writeHeader(const PboHeaderEntity& header) const;

    private:
        PboFile* file_;
    };
}
