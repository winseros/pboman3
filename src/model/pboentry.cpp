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
        return packingMethod == PboPackingMethod::product;
    }

    bool PboEntry::isCompressed() const {
        return packingMethod == PboPackingMethod::packed && originalSize != dataSize;
    }

    bool PboEntry::isContent() const {
        return !isBoundary() && packingMethod == PboPackingMethod::uncompressed ||
               packingMethod == PboPackingMethod::packed;
    }

    int PboEntry::size() const {
        return static_cast<int>(fileName.length()) + PboEntry::sizeOfFields;
    }

    //each header entry consists of 5x4 bytes of fields + filename.length + 1 byte zero string terminator
    const int PboEntry::sizeOfFields = 21;

    PboEntry_::PboEntry_(QString fileName, PboPackingMethod packingMethod,
                         int originalSize, int reserved,
                         int timestamp, int dataSize)
            : PboEntry(std::move(fileName), packingMethod,
                       originalSize, reserved,
                       timestamp, dataSize),
              dataOffset(0) {
    }
}