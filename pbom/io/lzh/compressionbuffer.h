#pragma once

#include <QFileDevice>

namespace pboman3::io {
    struct BufferIntersection {
        inline static qint64 posNo = -1;
        qint64 position;
        qint64 length;
    };

    struct SequenceInspection {
        qint64 sourceBytes;
        qint64 sequenceBytes;
    };

    class CompressionBuffer {
    public:
        inline static int defaultSize = 0b0000111111111111;

        CompressionBuffer(qint64 size = defaultSize);

        void add(QFileDevice* source, qint64 length);

        void add(char byte);

        BufferIntersection intersect(const QByteArray& buffer, qint64 length);

        qint64 checkWhitespace(QByteArray& buffer, qint64 length);

        SequenceInspection checkSequence(const QByteArray& buffer, qint64 length);

        qint64 getFulfillment() const;
    private:
        qint64 size_;
        qint64 fullfillment_;
        QByteArray data_;
        char* dataPtr_;

        BufferIntersection intersectBufferAtOffset(const QByteArray& buffer, qint64 bLength, qint64 offset);

        SequenceInspection checkSequenceImpl(const QByteArray& buffer, qint64 length, qint64 sequenceBytes);
    };
}
