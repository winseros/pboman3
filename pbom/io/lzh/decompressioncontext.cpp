#include "decompressioncontext.h"

namespace pboman3::io {
    DecompressionContext::DecompressionContext(QFileDevice* pSource, QFileDevice* pTarget)
        : format(0),
        crc(0),
        source(pSource),
        target(pTarget) {
        buffer.resize(18);
    }

    void DecompressionContext::write(char data) {
        target->write(&data, sizeof data);
        updateCrc(data);
    }

    void DecompressionContext::write(const QByteArray& data, int chunkSize) {
        target->write(data.data(), chunkSize);
        updateCrc(data, chunkSize);
    }

    void DecompressionContext::setBuffer(qint64 offset, int length) {
        const qint64 pos = target->pos();
        const bool seek = target->seek(offset);
        assert(seek);
        target->read(buffer.data(), length);
        target->seek(pos);
    }

    void DecompressionContext::updateCrc(char data) {
        crc = crc + static_cast<quint8>(data);
    }

    void DecompressionContext::updateCrc(const QByteArray& data, int chunkSize) {
        for (int i = 0; i < chunkSize; i++) {
            crc = crc + static_cast<quint8>(data[i]);
        }
    }
}
