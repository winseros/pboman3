#pragma once

#include <QFileDevice>
#include "pboioexception.h"
#include "util/util.h"

namespace pboman3 {
    class LzhDecompressionException : public PboIoException {
    public:
        LzhDecompressionException(QString message);

        friend QDebug operator<<(QDebug debug, const PboIoException& ex);

        void raise() const override;

        QException* clone() const override;
    private:
        const QString message_;
    };

    class Lzh {
    public:
        static void decompress(QFileDevice* source, QFileDevice* target, int outputLength, const Cancel& cancel);

    private:
        class ProcessContext {
        public:
            int format;
            uint crc;
            QByteArray buffer;
            QFileDevice* source;
            QFileDevice* target;

            ProcessContext(QFileDevice* pSource, QFileDevice* pTarget);

            void write(char data);

            void write(const QByteArray& data, int chunkSize);

            void setBuffer(qint64 offset, int length);

        private:
            void updateCrc(char data);

            void updateCrc(const QByteArray& data, int chunkSize);
        };

        static void processBlock(ProcessContext& ctx);

        static bool isValid(ProcessContext& ctx);
    };
}
