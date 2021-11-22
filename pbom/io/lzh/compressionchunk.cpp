#include "compressionchunk.h"

namespace pboman3::io {
    CompressionChunk::CompressionChunk()
        : format_(0b00000000),
          length_(0) {
        data_.resize(maxChunkSize_ * 2);
        next_.resize(maxChunkSize_);
    }

    qint64 CompressionChunk::compose(QFileDevice* source, CompressionBuffer& dict) {
        qint64 packedTotal = 0;

        for (qint8 i = 0; i < chunks_ && !source->atEnd(); i++) {
            const qint64 chunkSize = std::min(maxChunkSize_, source->size() - source->pos());
            qint64 packed;
            if (chunkSize < minBytesToPack_) {
                packed = composeUncompressed(i, source, dict);
            } else {
                packed = composeCompressed(i, source, chunkSize, dict);
            }
            packedTotal += packed;
            const bool seek = source->seek(source->pos() + packed);
            assert(seek);
        }

        return packedTotal;
    }

    int CompressionChunk::flush(QFileDevice* target) {
        target->write(reinterpret_cast<const char*>(&format_), sizeof format_);
        target->write(data_.data(), length_);
        return length_ + 1;
    }

    qint64 CompressionChunk::composeUncompressed(qint8 chunk, QFileDevice* source, CompressionBuffer& dict) {
        constexpr qint64 bytesToCopy = 1;
        char byte;
        source->peek(&byte, bytesToCopy);
        data_[length_] = byte;
        format_ += 1 << chunk;  // NOLINT(bugprone-narrowing-conversions, cppcoreguidelines-narrowing-conversions)
        length_ += bytesToCopy;

        dict.add(byte);

        return bytesToCopy;
    }

    qint64 CompressionChunk::composeCompressed(qint8 chunk, QFileDevice* source, qint64 chunkSize,
                                               CompressionBuffer& dict) {
        source->peek(next_.data(), chunkSize);

        const BufferIntersection intersect = dict.intersect(next_, chunkSize);
        const qint64 whitespace = source->pos() < maxOffsetToUseWhitespaces_
                                      ? dict.checkWhitespace(next_, chunkSize)
                                      : 0;
        const SequenceInspection sequence = dict.checkSequence(next_, chunkSize);

        qint64 processed;
        if (intersect.length >= minBytesToPack_ || whitespace >= minBytesToPack_ || sequence.sourceBytes >=
            minBytesToPack_) {
            qint16 pointer;
            if (intersect.length >= whitespace && intersect.length >= sequence.sourceBytes) {
                pointer = composePointer(dict.getFulfillment() - intersect.position, intersect.length);
                processed = intersect.length;
            } else if (whitespace >= intersect.length && whitespace >= sequence.sourceBytes) {
                pointer = composePointer(source->pos() + whitespace, whitespace);
                processed = whitespace;
            } else {
                pointer = composePointer(sequence.sequenceBytes, sequence.sourceBytes);
                processed = sequence.sourceBytes;
            }
            dict.add(source, processed);
            memcpy(data_.data() + length_, reinterpret_cast<char*>(&pointer), sizeof pointer);
            length_ += sizeof pointer;
        } else {
            processed = composeUncompressed(chunk, source, dict);
        }

        return processed;
    }

    qint16 CompressionChunk::composePointer(qint64 offset, qint64 length) {
        const auto vLength = static_cast<qint16>((length - minBytesToPack_) << 8);
        const auto vOffset = static_cast<qint16>(((offset & 0x0F00) << 4) + (offset & 0x00FF));
        const auto result = static_cast<qint16>(vOffset + vLength);
        return result;
    }
}
