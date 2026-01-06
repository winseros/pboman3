#include "createdocumentreader.h"
#include "jf/compositefilter.h"
#include "jf/dummyfilter.h"
#include "jf/zerosizefilter.h"
#include "../settings/getsettingsfacility.h"

namespace pboman3::io {
    DocumentReader CreateDocumentReader(QString path, const bool junkFilterEnable) {
        return DocumentReader(
            std::move(path),
            QSharedPointer<JunkFilter>(
                junkFilterEnable
                    ? dynamic_cast<JunkFilter*>(new CompositeFilter({QSharedPointer<JunkFilter>(new ZeroSizeFilter())}))
                    : dynamic_cast<JunkFilter*>(new DummyFilter)));
    }
}
