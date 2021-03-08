#include "pboparcel.h"

namespace pboman3 {

    int PboParcelItem::size() const {
        return static_cast<int>(path.length() + file.length() + sizeof
            dataOffset + sizeof
            originalSize + sizeof dataSize);
    }

    QDataStream& operator<<(QDataStream& stream, const PboParcelItem& item) {
        stream << item.path;
        stream << item.file;
        stream << item.dataOffset;
        stream << item.originalSize;
        stream << item.dataSize;
        return stream;
    }

    QDataStream& operator>>(QDataStream& stream, PboParcelItem& item) {
        stream >> item.path;
        stream >> item.file;
        stream >> item.dataOffset;
        stream >> item.originalSize;
        stream >> item.dataSize;
        return stream;
    }

    PboParcel PboParcel::deserialize(const QByteArray& data) {
        QDataStream stream(data);
        short length;
        stream >> length;

        PboParcel result;
        result.reserve(length);

        for (short i = 0; i < length; i++) {
            PboParcelItem item;
            stream >> item;
            result.append(item);
        }

        return result;
    }

    QByteArray PboParcel::serialize() {
        constexpr int noOfItemsBytes = 2;
        const qsizetype length = std::accumulate(begin(), end(), static_cast<qsizetype>(0),
                                                 [](qsizetype accumulated,
                                                    const PboParcelItem& parcel) {
                                                     return accumulated + parcel.size();
                                                 }) + noOfItemsBytes;
        QByteArray result;
        result.reserve(length);

        QDataStream stream(&result, QIODeviceBase::ReadWrite);
        stream << static_cast<qint16>(this->length());

        auto it = begin();
        while (it != end()) {
            stream << *it;
            ++it;
        }

        return result;
    }
}
