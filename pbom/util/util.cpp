#include "util.h"

namespace pboman3 {
    QString GetFileExtension(const QString& fileName) {
        const qsizetype extPos = fileName.lastIndexOf(".");
        QString ext = extPos >= 0 ? fileName.right(fileName.length() - extPos - 1) : "";
        return ext;
    }

    QString GetFileNameWithoutExtension(const QString& fileName) {
        const qsizetype extPos = fileName.lastIndexOf(".");
        QString ext = extPos > 0 ? fileName.left(extPos) : fileName;
        return ext;
    }
}
