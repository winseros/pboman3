#include "pboheaderentity.h"

namespace pboman3::io {
    PboHeaderEntity PboHeaderEntity::makeBoundary() {
        return {QString(), QString()};
    }

    PboHeaderEntity::PboHeaderEntity(QString name, QString value)
            : name(std::move(name)),
              value(std::move(value)) {
    }

    bool PboHeaderEntity::isBoundary() const {
        return name.isEmpty() && value.isEmpty();
    }

}
