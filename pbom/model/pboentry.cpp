#include "pboentry.h"

namespace pboman3 {
    PboEntry PboEntry::makeSignature() {
        return PboEntry("", PboPackingMethod::Product, 0, 0, 0, 0);
    }

    PboEntry PboEntry::makeBoundary() {
        return PboEntry("", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
    }

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
        return packingMethod == PboPackingMethod::Product;
    }

    bool PboEntry::isCompressed() const {
        return packingMethod == PboPackingMethod::Packed && originalSize != dataSize;
    }

    bool PboEntry::isContent() const {
        return !isBoundary() && packingMethod == PboPackingMethod::Uncompressed ||
               packingMethod == PboPackingMethod::Packed;
    }

    int PboEntry::size() const {
        return static_cast<int>(fileName.length()) + sizeOfFields;
    }

    PboPath PboEntry::makePath() const {
        return PboPath(fileName);
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
