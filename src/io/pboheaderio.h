#pragma once

#include <QFile>
#include <QPointer>
#include "model/pboentry.h"
#include "model/pboheader.h"
#include "pbofile.h"
#include "pbodatastream.h"

namespace pboman3 {
    using namespace std;

    class PboHeaderIO {
    public:
        explicit PboHeaderIO(PboFile* file);

        unique_ptr<PboEntry_> readNextEntry();

        unique_ptr<PboHeader> readNextHeader();

    private:
        PboFile* file_;
    };
}