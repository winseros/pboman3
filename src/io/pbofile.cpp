#include "pbofile.h"

namespace pboman3 {
    PboFile::PboFile(const QString& name)
        : QFile(name) {
    }

    int PboFile::readCString(QString& value) {
        int len = 0;
        startTransaction();
        while (!atEnd()) {
            char data;
            read(&data, sizeof(data));
            if (data == 0)
                break;
            len++;
        }
        rollbackTransaction();

        if (len) {
            QByteArray bytes(len, Qt::Initialization::Uninitialized);
            read(bytes.data(), len);
            value.append(bytes);
        }
        if (!atEnd())
            seek(pos() + 1);
        return len;
    }

    int PboFile::writeCString(const QString& value) {
        write(value.toUtf8());
        char zero = 0;
        write(&zero, sizeof zero);
        return value.length() + sizeof zero;
    }
}
