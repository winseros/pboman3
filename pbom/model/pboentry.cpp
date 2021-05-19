#include "pboentry.h"

namespace pboman3 {
    PboEntry PboEntry::makeSignature() {
        return PboEntry("", PboPackingMethod::Product, 0, 0, 0, 0);
    }

    PboEntry PboEntry::makeBoundary() {
        return PboEntry("", PboPackingMethod::Uncompressed, 0, 0, 0, 0);
    }

    PboEntry::PboEntry(QString fileName, PboPackingMethod packingMethod,
                       qint32 originalSize, qint32 reserved,
                       qint32 timestamp, qint32 dataSize) :
            fileName_(std::move(fileName)),
            packingMethod_(packingMethod),
            originalSize_(originalSize),
            reserved_(reserved),
            timestamp_(timestamp),
            dataSize_(dataSize) {
    }

    bool PboEntry::isBoundary() const {
        return fileName_.isEmpty();
    }

    bool PboEntry::isSignature() const {
        return packingMethod_ == PboPackingMethod::Product;
    }

    bool PboEntry::isCompressed() const {
        return packingMethod_ == PboPackingMethod::Packed && originalSize_ != dataSize_;
    }

    bool PboEntry::isContent() const {
        return !isBoundary() && packingMethod_ == PboPackingMethod::Uncompressed ||
               packingMethod_ == PboPackingMethod::Packed;
    }

    int PboEntry::size() const {
        return static_cast<int>(fileName_.length()) + sizeOfFields;
    }

    PboPath PboEntry::makePath() const {
        return PboPath(fileName_);
    }

    //each header entry consists of 5x4 bytes of fields + filename.length + 1 byte zero string terminator
    const int PboEntry::sizeOfFields = 21;


    const QString& PboEntry::fileName() const {
        return fileName_;
    }

    PboPackingMethod PboEntry::packingMethod() const {
        return packingMethod_;
    }

    qint32 PboEntry::originalSize() const {
        return originalSize_;
    }

    qint32 PboEntry::reserved() const {
        return reserved_;
    }

    qint32 PboEntry::timestamp() const {
        return timestamp_;
    }

    qint32 PboEntry::dataSize() const {
        return dataSize_;
    }
}
