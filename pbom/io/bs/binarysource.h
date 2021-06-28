#pragma once

#include <QDebug>
#include <QFileDevice>
#include "util/util.h"

namespace pboman3 {
    class BinarySource {
    public:
        BinarySource(QString path);

        virtual ~BinarySource();

        virtual void writeToPbo(QFileDevice* targetFile, const Cancel& cancel) = 0;

        virtual void writeToFs(QFileDevice* targetFile, const Cancel& cancel) = 0;

        void open() const;

        void close() const;

        bool isOpen() const;

        const QString& path() const;

        virtual quint32 readOriginalSize() const = 0;

        virtual quint32 readTimestamp() const = 0;

        virtual bool isCompressed() const = 0;

        friend QDebug operator <<(QDebug debug, const BinarySource& bs) {
            return debug << "BinarySource(Compressed=" << bs.isCompressed() << ", Path=" << bs.path_ << ")";
        }

    protected:
        QFileDevice* file_;

    private:
        QString path_;
    };
}
