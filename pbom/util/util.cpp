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

    void SplitByNameAndExtension(const QString& fileName, QString& outFileName, QString& outExtension) {
        const qsizetype extPos = fileName.lastIndexOf(".");
        outFileName = extPos > 0 ? fileName.left(extPos) : fileName;
        outExtension = fileName.length() == outFileName.length()
                           ? ""
                           : fileName.right(fileName.length() - outFileName.length() - 1);
    }
}
