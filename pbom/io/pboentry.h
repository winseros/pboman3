#pragma once

#include "domain/pbopath.h"

namespace pboman3::io {
    using namespace std;
    using namespace domain;

    enum class PboPackingMethod {
        Uncompressed = 0x00000000,
        Packed = 0x43707273,
        Product = 0x56657273
    };

    class PboEntry {
    public:
        static PboEntry makeSignature();

        static PboEntry makeBoundary();

        PboEntry(QString fileName, PboPackingMethod packingMethod,
                 qint32 originalSize, qint32 reserved,
                 qint32 timestamp, qint32 dataSize);

        virtual ~PboEntry() = default;

        bool isBoundary() const;

        bool isSignature() const;

        bool isCompressed() const;

        bool isContent() const;

        int size() const;

        PboPath makePath() const;

        const QString& fileName() const;

        PboPackingMethod packingMethod() const;

        qint32 originalSize() const;

        qint32 reserved() const;

        qint32 timestamp() const;

        qint32 dataSize() const;

        friend QDebug operator <<(QDebug debug, const PboEntry& entry);

    private:
        static const int sizeOfFields;

        QString fileName_;
        PboPackingMethod packingMethod_;
        qint32 originalSize_;
        qint32 reserved_;
        qint32 timestamp_;
        qint32 dataSize_;
    };
}
