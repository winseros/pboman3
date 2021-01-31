#include <memory>
#include "pboheaderio.h"

namespace pboman3 {
    using namespace std;

    PboHeaderIO::PboHeaderIO(PboFile* file)
            : file_(file) {
    }

    unique_ptr<PboEntry_> PboHeaderIO::readNextEntry() {
        PboDataStream data(file_);
#define _EXIT if (data.atEnd()) { return nullptr; }

        _EXIT
        QString fileName;
        data >> fileName;

        _EXIT
        PboPackingMethod packingMethod;
        data >> packingMethod;

        _EXIT
        int originalSize;
        data >> originalSize;

        _EXIT
        int reserved;
        data >> reserved;

        _EXIT
        int timeStamp;
        data >> timeStamp;

        _EXIT
        long dataSize;
        data >> dataSize;

        return make_unique<PboEntry_>(fileName, packingMethod, originalSize, reserved, timeStamp, dataSize);
    }

    unique_ptr<PboHeader> PboHeaderIO::readNextHeader() {
        PboDataStream data(file_);
#define _EXIT if (data.atEnd()) { return nullptr; }

        _EXIT
        QString name;
        data >> name;

        if (name.isEmpty())
            return make_unique<PboHeader>(QString(), QString());

        _EXIT
        QString value;
        data >> value;

        return make_unique<PboHeader>(name, value);
    }

}