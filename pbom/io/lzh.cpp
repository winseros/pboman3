#include "lzh.h"

namespace pboman3 {
    LzhDecompressionException::LzhDecompressionException(QString message):
        PboIoException(std::move(message)) {
    }

    QDebug operator<<(QDebug debug, const LzhDecompressionException& ex) {
        return debug << "LzhDecompressionException: " << ex.message();
    }

    void LzhDecompressionException::raise() const {
        throw *this;
    }

    QException* LzhDecompressionException::clone() const {
        return new LzhDecompressionException(*this);
    }

    void Lzh::decompress(QFileDevice* source, QFileDevice* target, int outputLength, const Cancel& cancel) {
        ProcessContext ctx(source, target);
        const qint64 maxTargetOffset = target->pos() + outputLength;
        const qint64 maxSourceOffset = source->size() - 2;
        while (target->pos() < maxTargetOffset && !source->atEnd() && !cancel()) {
            char format;
            source->read(&format, sizeof format);
            for (char i = 0; i < 8 && target->pos() < maxTargetOffset && source->pos() < maxSourceOffset; i++) {
                ctx.format = format >> i & 0x01;
                processBlock(ctx);
            }
        }
        const bool valid = isValid(ctx);
        if (!valid)
            throw LzhDecompressionException("Could not decompress the file");
    }

    void Lzh::processBlock(ProcessContext& ctx) {
        constexpr int packetFormatUncompressed = 1;
        constexpr char whitespaceChar = 0x20;

        if (ctx.format == packetFormatUncompressed) {
            char data;
            ctx.source->read(&data, sizeof data);
            ctx.write(data);
        } else {
            qint16 pointer;
            ctx.source->read(reinterpret_cast<char*>(&pointer), sizeof pointer);
            qint64 rpos = ctx.target->pos() - static_cast<qint64>((pointer & 0x00ff)) - static_cast<qint64>(((pointer & 0xf000) >> 4));
            int rlen = ((pointer & 0x0f00) >> 8) + 3;

            if (rpos + rlen < 0) {
                for (int i = 0; i < rlen; i++)
                    ctx.write(whitespaceChar);
            } else {
                while (rpos < 0) {
                    ctx.write(whitespaceChar);
                    rpos++;
                    rlen--;
                }
                if (rlen > 0) {
                    const int chunkSize = rpos + rlen > ctx.target->pos()
                                               ? static_cast<int>(ctx.target->pos() - rpos)
                                               : rlen;
                    ctx.setBuffer(rpos, chunkSize);

                    while (rlen >= chunkSize && chunkSize > 0) {
                        ctx.write(ctx.buffer, chunkSize);
                        rlen -= chunkSize;
                    }
                    for (int j = 0; j < rlen; j++) {
                        ctx.write(ctx.buffer[j]);
                    }
                }
            }
        }
    }

    bool Lzh::isValid(ProcessContext& ctx) {
        bool valid = false;

        if (ctx.source->size() - ctx.source->pos() >= static_cast<qint32>(sizeof(uint))) {
            uint crc;
            ctx.source->read(reinterpret_cast<char*>(&crc), sizeof crc);
            valid = crc == ctx.crc;
        }

        return valid;
    }

    Lzh::ProcessContext::ProcessContext(QFileDevice* pSource, QFileDevice* pTarget)
        : format(0),
          crc(0),
          source(pSource),
          target(pTarget) {
        buffer.resize(18);
    }

    void Lzh::ProcessContext::write(char data) {
        target->write(&data, sizeof data);
        updateCrc(data);
    }

    void Lzh::ProcessContext::write(const QByteArray& data, int chunkSize) {
        target->write(data.data(), chunkSize);
        updateCrc(data, chunkSize);
    }

    void Lzh::ProcessContext::setBuffer(qint64 offset, int length) {
        const qint64 pos = target->pos();
        target->seek(offset);
        target->read(buffer.data(), length);
        target->seek(pos);
    }

    void Lzh::ProcessContext::updateCrc(char data) {
        crc = crc + static_cast<quint8>(data);
    }

    void Lzh::ProcessContext::updateCrc(const QByteArray& data, int chunkSize) {
        for (int i = 0; i < chunkSize; i++) {
            crc = crc + static_cast<quint8>(data[i]);
        }
    }
}
