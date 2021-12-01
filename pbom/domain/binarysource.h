#pragma once

#include <QDebug>
#include <QFileDevice>
#include "util/util.h"

namespace pboman3::domain {
    using namespace util;

    class BinarySource {

    protected:
        ~BinarySource() = default;

    public:
        virtual void writeToPbo(QFileDevice* targetFile, const Cancel& cancel) = 0;

        virtual void writeToFs(QFileDevice* targetFile, const Cancel& cancel) = 0;

        virtual void open() const = 0;

        virtual void close() const = 0;

        virtual bool isOpen() const = 0;

        virtual const QString& path() const = 0;

        virtual qint32 readOriginalSize() const = 0;

        virtual qint32 readTimestamp() const = 0;

        virtual bool isCompressed() const = 0;

        friend QDebug operator <<(QDebug debug, const BinarySource& bs) {
            return debug << "BinarySource()";
        }
    };
}
