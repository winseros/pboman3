#include "pboheaderio.h"
#include <memory>
#include "pbodatastream.h"

namespace pboman3 {
    using namespace std;

    PboHeaderIO::PboHeaderIO(PboFile* file)
        : file_(file) {
    }

    unique_ptr<PboEntry_> PboHeaderIO::readNextEntry() const {
        PboDataStream data(file_);

        try {
            QString fileName;
            data >> fileName;

            PboPackingMethod packingMethod;
            data >> packingMethod;

            int originalSize;
            data >> originalSize;

            int reserved;
            data >> reserved;

            int timeStamp;
            data >> timeStamp;

            long dataSize;
            data >> dataSize;

            return make_unique<PboEntry_>(fileName, packingMethod, originalSize, reserved, timeStamp, dataSize);
        } catch (PboEofException&) {
            return nullptr;
        }
    }

    unique_ptr<PboHeader> PboHeaderIO::readNextHeader() const {
        PboDataStream data(file_);

        try {
            QString name;
            data >> name;

            if (name.isEmpty())
                return make_unique<PboHeader>(QString(), QString());

            QString value;
            data >> value;

            return make_unique<PboHeader>(name, value);
        } catch (PboEofException&) {
            return nullptr;
        }
    }

    void PboHeaderIO::writeEntry(const PboEntry* entry) const {
        PboDataStream data(file_);

        data << entry->fileName;
        data << entry->packingMethod;
        data << entry->originalSize;
        data << entry->reserved;
        data << entry->timestamp;
        data << entry->dataSize;
    }

    void PboHeaderIO::writeHeader(const PboHeader* header) const {
        PboDataStream data(file_);

        if (header->isBoundary()) {
            data << nullptr;
        } else {
            data << header->name;
            data << header->value;
        }
    }

}
