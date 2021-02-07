#include "changestream.h"

namespace pboman3 {

    ChangeAdd::ChangeAdd(QString filePath, QString pboPath)
        : systemFilePath(std::move(filePath)),
          pboFilePath(std::move(pboPath)) {
    }
}
