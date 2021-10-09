#include "compressionbuffer.h"
#include <cstring>

namespace pboman3 {
    CompressionBuffer::CompressionBuffer(qint64 size)
        : size_(size),
          fullfillment_(0) {
        data_.resize(size_);
        dataPtr_ = data_.data();
    }

    void CompressionBuffer::add(QFileDevice* source, qint64 length) {
        if (fullfillment_ + length > size_) {
            //shift the buffer contents left until there is enough space for the new bunch of bytes
            const qint64 bytesToMoveCount = size_ - length;
            const char* bytesToMove = data_.data() + fullfillment_ - bytesToMoveCount;
            std::memmove(dataPtr_, bytesToMove, bytesToMoveCount);
            source->peek(dataPtr_ + bytesToMoveCount, length);
            fullfillment_ = size_;
        } else {
            //add some bytes onto the free space of the buffer
            source->peek(dataPtr_ + fullfillment_, length);
            fullfillment_ += length;
        }
    }

    void CompressionBuffer::add(char byte) {
        constexpr uint length = sizeof byte;
        if (fullfillment_ + length > size_) {
            //shift the buffer contents left until there is enough space for the new bunch of bytes
            const qint64 bytesToMoveCount = size_ - length;
            const char* bytesToMove = data_.data() + fullfillment_ - bytesToMoveCount;
            std::memmove(dataPtr_, bytesToMove, bytesToMoveCount);
            data_[bytesToMoveCount] = byte;
            fullfillment_ = size_;
        } else {
            //add some bytes onto the free space of the buffer
            data_[fullfillment_] = byte;
            fullfillment_ += length;
        }
    }

    BufferIntersection CompressionBuffer::intersect(const QByteArray& buffer, qint64 length) {
        if (length && fullfillment_) {
            qint64 offset = 0;
            BufferIntersection intersection{BufferIntersection::posNo, 0};
            while (true) {
                const BufferIntersection next = intersectBufferAtOffset(buffer, length, offset);
                if (next.position >= 0 && intersection.length < next.length) {
                    intersection = next;
                }
                if (next.position >= 0 && next.position <= fullfillment_ - 1) {
                    offset = next.position + 1;
                } else {
                    break;
                }
            }
            return intersection;
        }
        return BufferIntersection{BufferIntersection::posNo, 0};
    }

    qint64 CompressionBuffer::checkWhitespace(QByteArray& buffer, qint64 length) {
        qint64 count = 0;
        for (qint64 i = 0; i < length; i++) {
            if (buffer[i] == 0x20) {
                count++;
            } else {
                break;
            }
        }
        return count;
    }

    SequenceInspection CompressionBuffer::checkSequence(const QByteArray& buffer, qint64 length) {
        SequenceInspection result{0, 0};
        const qint64 maxSourceBytes = std::min(fullfillment_, length);
        for (qint64 i = 1; i < maxSourceBytes; i++) {
            const SequenceInspection sequence = checkSequenceImpl(buffer, length, i);
            if (sequence.sourceBytes > result.sourceBytes) {
                result = sequence;
            }
        }
        return result;
    }

    qint64 CompressionBuffer::getFulfillment() const {
        return fullfillment_;
    }

    BufferIntersection CompressionBuffer::intersectBufferAtOffset(const QByteArray& buffer, qint64 bLength,
                                                                  qint64 offset) {
        const qint64 position = data_.indexOf(buffer[0], offset);
        qint64 length = 0;
        if (position >= 0 && position < fullfillment_) {
            length++;
            for (qint64 bufIndex = 1, dataIndex = position + 1; bufIndex < bLength && dataIndex < fullfillment_;
                 bufIndex++, dataIndex++) {
                if (data_[dataIndex] == buffer[bufIndex]) {
                    length++;
                } else {
                    break;
                }
            }
        }
        return BufferIntersection{position, length};
    }

    SequenceInspection CompressionBuffer::checkSequenceImpl(const QByteArray& buffer, qint64 length,
                                                            qint64 sequenceBytes) {
        qint64 sourceBytes = 0;
        while (sourceBytes < length) {
            for (qint64 i = fullfillment_ - sequenceBytes; i < fullfillment_ && sourceBytes < length; i++) {
                if (buffer[sourceBytes] == data_[i]) {
                    sourceBytes++;
                } else {
                    return SequenceInspection{sourceBytes, sequenceBytes};
                }
            }
        }

        return SequenceInspection{sourceBytes, sequenceBytes};
    }
}
