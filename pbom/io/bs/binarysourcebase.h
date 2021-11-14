#pragma once

#include "domain/binarysource.h"

namespace pboman3::io {
    class BinarySourceBase: public BinarySource {
    public:
        BinarySourceBase(QString path);

        virtual ~BinarySourceBase();

        virtual void writeToPbo(QFileDevice* targetFile, const Cancel& cancel) = 0;

        virtual void writeToFs(QFileDevice* targetFile, const Cancel& cancel) = 0;

        void open() const override;

        void close() const override;

        bool isOpen() const override;

        const QString& path() const override;

        qint32 readOriginalSize() const override = 0;

        qint32 readTimestamp() const override = 0;

        bool isCompressed() const override = 0;

        friend QDebug operator <<(QDebug debug, const BinarySourceBase& bs) {
            return debug << "BinarySourceBase(Compressed=" << bs.isCompressed() << ", Path=" << bs.path_ << ")";
        }

    protected:
        QFileDevice* file_;

    private:
        QString path_;
    };
}
