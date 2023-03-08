#include "defaultdocumentreaderfactory.h"
#include "jf/compositefilter.h"
#include "jf/zerosizefilter.h"

namespace pboman3::io {
    DocumentReader DefaultDocumentReaderFactory::createDocumentReader(QString path) {
        return DocumentReader(
            std::move(path),
            QSharedPointer<JunkFilter>(new CompositeFilter({
                QSharedPointer<JunkFilter>(new ZeroSizeFilter())
            })));
    }
}
