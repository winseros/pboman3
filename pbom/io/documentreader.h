#pragma once

#include "domain/pbodocument.h"
#include "io/jf/junkfilter.h"

namespace pboman3::io {
    using namespace domain;

    class DocumentReader {
    public:
        DocumentReader(QString path, QSharedPointer<JunkFilter> junkFilter);

        QSharedPointer<PboDocument> read() const;

    private:
        QString path_;
        QSharedPointer<JunkFilter> junkFilter_;
    };
}
