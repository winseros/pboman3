#pragma once

#include <QFileDevice>
#include "util/util.h"

namespace pboman3 {
    class BinarySource {
    public:
        BinarySource(const QString& path);

        virtual ~BinarySource();

        virtual void writeDecompressed(QFileDevice* targetFile, const Cancel& cancel) = 0;

        virtual void writeCompressed(QFileDevice* targetFile, const Cancel& cancel) = 0;

        virtual void writeRaw(QFileDevice* targetFile, const Cancel& cancel) = 0;

        const QString& path() const;

    protected:
        QFileDevice* file_;

    private:
        QString path_;
    };

    
}
