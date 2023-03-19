#pragma once

#include "domain/binarysource.h"

namespace pboman3::io {
    using namespace domain;

    class AbstractBinarySource: public BinarySource {
    public:
        AbstractBinarySource(QString path);

        virtual ~AbstractBinarySource();

        void writeToPbo(QFileDevice* targetFile, const Cancel& cancel) override = 0;

        void writeToFs(QFileDevice* targetFile, const Cancel& cancel) override = 0;

        void open() const override;

        void close() const override;

        bool isOpen() const override;

        const QString& path() const override;

        qint32 readOriginalSize() const override = 0;

        qint32 readTimestamp() const override = 0;

        bool isCompressed() const override = 0;

        friend QDebug operator <<(QDebug debug, const AbstractBinarySource& bs) {
            return debug << "AbstractBinarySource(Compressed=" << bs.isCompressed() << ", Path=" << bs.path_ << ")";
        }

    protected:
        QFileDevice* file_;

    private:
        QString path_;
    };
}
