#include "pboentry.h"

namespace pboman3 {
    PboEntry::PboEntry(QString fileName, PboPackingMethod packingMethod,
                       int originalSize, int reserved,
                       int timestamp, int dataSize) :
            fileName(std::move(fileName)),
            packingMethod(packingMethod),
            originalSize(originalSize),
            reserved(reserved),
            timestamp(timestamp),
            dataSize(dataSize) {
    }

    bool PboEntry::isBoundary() const {
        return fileName.isEmpty();
    }

    bool PboEntry::isSignature() const {
        return packingMethod == product;
    }

    bool PboEntry::isCompressed() const {
        return packingMethod == packed && originalSize != dataSize;
    }

    bool PboEntry::isContent() const {
        return !isBoundary() && packingMethod == uncompressed ||
               packingMethod == packed;
    }

    int PboEntry::size() const {
        return static_cast<int>(fileName.length()) + sizeOfFields;
    }

    //each header entry consists of 5x4 bytes of fields + filename.length + 1 byte zero string terminator
    const int PboEntry::sizeOfFields = 21;

    PboEntry_::PboEntry_(QString pFileName, PboPackingMethod pPackingMethod,
                         int pOriginalSize, int pReserved,
                         int pTimestamp, int pDataSize)
            : PboEntry(std::move(pFileName), pPackingMethod,
                       pOriginalSize, pReserved,
                       pTimestamp, pDataSize),
              dataOffset(0) {
    }
}