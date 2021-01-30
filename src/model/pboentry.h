#pragma once

#include <QString>
#include <memory>

namespace pboman3 {
    using namespace std;

    enum PboPackingMethod {
        uncompressed = 0x00000000,
        packed = 0x43707273,
        product = 0x56657273
    };

    struct PboEntry {
        const QString fileName;
        const PboPackingMethod packingMethod;
        const int originalSize;
        const int reserved;
        const int timestamp;
        const int dataSize;

        PboEntry(QString fileName, PboPackingMethod packingMethod,
                 int originalSize, int reserved,
                 int timestamp, int dataSize);

        bool isBoundary() const;

        bool isSignature() const;

        bool isCompressed() const;

        bool isContent() const;

        int size() const;

    private:
        static const int sizeOfFields;
    };

    struct PboEntry_ : public PboEntry {
        long dataOffset;

        PboEntry_(QString fileName, PboPackingMethod packingMethod,
                  int originalSize, int reserved,
                  int timestamp, int dataSize);
    };
}
