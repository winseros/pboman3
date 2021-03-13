#pragma once

#include <QFileDevice>

namespace pboman3 {
    class DecompressionContext {
    public:
        int format;
        uint crc;
        QByteArray buffer;
        QFileDevice* source;
        QFileDevice* target;

        DecompressionContext(QFileDevice* pSource, QFileDevice* pTarget);

        void write(char data);

        void write(const QByteArray& data, int chunkSize);

        void setBuffer(qint64 offset, int length);

    private:
        void updateCrc(char data);

        void updateCrc(const QByteArray& data, int chunkSize);
    };
}
