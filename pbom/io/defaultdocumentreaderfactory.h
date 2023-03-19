#pragma once

#include "documentreader.h"

namespace pboman3::io {
    class DefaultDocumentReaderFactory
    {
    public:
        static DocumentReader createDocumentReader(QString path);
    };
}
