#pragma once

#include "domain/pbodocument.h"

namespace pboman3::io {
    using namespace domain;

    class DocumentReader {
    public:
        DocumentReader(QString path);

        QSharedPointer<PboDocument> read() const;

    private:
        QString path_;
    };
}
