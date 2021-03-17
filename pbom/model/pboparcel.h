#pragma once

#include <QList>

namespace pboman3 {
    struct PboParcelItem {
        QString path;

        QString file;

        size_t dataOffset;

        int dataSize;

        int originalSize;

        int size() const;

        friend QDataStream& operator << (QDataStream& stream, const PboParcelItem& item);

        friend QDataStream& operator >> (QDataStream& stream, PboParcelItem& item);
    };

    class PboParcel : public QList<PboParcelItem> {
    public:
        static PboParcel deserialize(const QByteArray& data);

        QByteArray serialize();
    };
}
