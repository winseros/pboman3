#pragma once

#include "compressionbuffer.h"

namespace pboman3 {
    class CompressionChunk {
    public:
        CompressionChunk();

        qint64 compose(QFileDevice* source, CompressionBuffer& dict);

        int flush(QFileDevice* target);
    private:
        inline static qint8 chunks_ = 8;
        inline static qint64 minBytesToPack_ = 3;
        inline static qint64 maxChunkSize_ = minBytesToPack_ + 0b1111;
        inline static qint64 maxOffsetToUseWhitespaces_ = CompressionBuffer::defaultSize - maxChunkSize_;

        QByteArray data_;
        QByteArray next_;
        qint8 format_;
        int length_;

        qint64 composeUncompressed(qint8 chunk, QFileDevice* source, CompressionBuffer& dict);

        qint64 composeCompressed(qint8 chunk, QFileDevice* source, qint64 chunkSize, CompressionBuffer& dict);

        qint16 composePointer(qint64 offset, qint64 length);
    };
}
