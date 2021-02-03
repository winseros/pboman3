#include "changestream.h"

namespace pboman3 {

    ChangeAdd::ChangeAdd(QString filePath, QString pboPath)
        : systemFilePath(std::move(filePath)),
          pboFilePath(std::move(pboPath)) {
    }

    ChangeBase::ChangeBase(const PboEntry_* entry)
        : entry(entry) {
    }

    ChangeMove::ChangeMove(const PboEntry_* entry, QString newPboPath)
        : ChangeBase(entry),
          pboFilePath(std::move(newPboPath)) {
    }

    ChangeDelete::ChangeDelete(const PboEntry_* entry)
        : ChangeBase(entry) {
    }
}
