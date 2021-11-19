#include "pboheader.h"

namespace pboman3::io {
    PboHeader PboHeader::makeBoundary() {
        return PboHeader(QString(), QString());
    }

    PboHeader::PboHeader(QString name, QString value)
            : name(std::move(name)),
              value(std::move(value)) {
    }

    bool PboHeader::isBoundary() const {
        return name.isEmpty() && value.isEmpty();
    }

}
