#pragma once

#include <QFileDevice>
#include "util/util.h"

namespace pboman3 {
    class BinarySource {
    public:
        BinarySource(QString path);

        virtual ~BinarySource();

        virtual void writeToPbo(QFileDevice* targetFile, const Cancel& cancel) = 0;

        virtual void writeToFs(QFileDevice* targetFile, const Cancel& cancel) = 0;

        const QString& path() const;

    protected:
        QFileDevice* file_;

    private:
        QString path_;
    };

    
}
