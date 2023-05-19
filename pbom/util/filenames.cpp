#include "filenames.h"

namespace pboman3::util {
    QString FileNames::getCopyFolderName(const QString& desiredFolderName, int suffix) {
        return desiredFolderName + "(" + QString::number(suffix) + ")";
    }

    QString FileNames::getCopyFileName(const QString& desiredFileName, int suffix) {
        QString expectedName, expectedExt;
        splitByNameAndExtension(desiredFileName, expectedName, expectedExt);
        return expectedExt.isEmpty()
                   ? expectedName + "(" + QString::number(suffix) + ")"
                   : expectedName + "(" + QString::number(suffix) + ")." + expectedExt;
    }

    QString FileNames::getFileExtension(const QString& fileName) {
        const qsizetype extPos = fileName.lastIndexOf(".");
        QString ext = extPos >= 0 ? fileName.right(fileName.length() - extPos - 1) : "";
        return ext;
    }

    QString FileNames::getFileNameWithoutExtension(const QString& fileName) {
        const qsizetype extPos = fileName.lastIndexOf(".");
        QString ext = extPos > 0 ? fileName.left(extPos) : fileName;
        return ext;
    }

    void FileNames::splitByNameAndExtension(const QString& fileName, QString& outFileName, QString& outExtension) {
        const qsizetype extPos = fileName.lastIndexOf(".");
        outFileName = extPos > 0 ? fileName.left(extPos) : fileName;
        outExtension = fileName.length() == outFileName.length()
                           ? ""
                           : fileName.right(fileName.length() - outFileName.length() - 1);
    }
}
