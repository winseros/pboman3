#include "pbonodeentity.h"
#include <QDebug>

namespace pboman3::io {
    PboNodeEntity PboNodeEntity::makeSignature() {
        return {"", PboPackingMethod::Product, 0, 0, 0, 0};
    }

    PboNodeEntity PboNodeEntity::makeBoundary() {
        return {"", PboPackingMethod::Uncompressed, 0, 0, 0, 0};
    }

    PboNodeEntity::PboNodeEntity(QString fileName, PboPackingMethod packingMethod,
                       qint32 originalSize, qint32 reserved,
                       qint32 timestamp, qint32 dataSize) :
        fileName_(std::move(fileName)),
        packingMethod_(packingMethod),
        originalSize_(originalSize),
        reserved_(reserved),
        timestamp_(timestamp),
        dataSize_(dataSize) {
    }

    bool PboNodeEntity::isBoundary() const {
        return fileName_.isEmpty();
    }

    bool PboNodeEntity::isSignature() const {
        return packingMethod_ == PboPackingMethod::Product;
    }

    bool PboNodeEntity::isCompressed() const {
        return packingMethod_ == PboPackingMethod::Packed && originalSize_ != dataSize_;
    }

    bool PboNodeEntity::isContent() const {
        return !isBoundary() && packingMethod_ == PboPackingMethod::Uncompressed ||
            packingMethod_ == PboPackingMethod::Packed;
    }

    int PboNodeEntity::size() const {
        return static_cast<int>(fileName_.length()) + sizeOfFields;
    }

    PboPath PboNodeEntity::makePath() const {
        return PboPath(fileName_);
    }

    //each header entry consists of 5x4 bytes of fields + filename.length + 1 byte zero string terminator
    constexpr int PboNodeEntity::sizeOfFields = 21;

    const QString& PboNodeEntity::fileName() const {
        return fileName_;
    }

    PboPackingMethod PboNodeEntity::packingMethod() const {
        return packingMethod_;
    }

    qint32 PboNodeEntity::originalSize() const {
        return originalSize_;
    }

    qint32 PboNodeEntity::reserved() const {
        return reserved_;
    }

    qint32 PboNodeEntity::timestamp() const {
        return timestamp_;
    }

    qint32 PboNodeEntity::dataSize() const {
        return dataSize_;
    }

    QDebug operator<<(QDebug debug, const PboNodeEntity& entry) {
        return debug << "PboNodeEntity(FileName=" << entry.fileName_ << ", PackingMethod=" <<
            static_cast<qint32>(entry.packingMethod_) << ", OriginalSize=" << entry.originalSize_ << ", TimeStamp=" <<
            entry.timestamp_ << ", DataSize=" << entry.dataSize_ << ")";
    }
}
