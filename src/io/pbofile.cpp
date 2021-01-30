#include "pbofile.h"

namespace pboman3
{
    int PboFile::readCString(QString &value) {
        int len = 0;
        startTransaction();
        while (!atEnd()) {
            char data;
            read(&data, sizeof(data));
            if (data == 0) break;
            len++;
        }
        rollbackTransaction();

        if (len) {
            QByteArray bytes(len, Qt::Initialization::Uninitialized);
            read(bytes.data(), len);
            seek(pos() + 1);
            value.append(bytes);
        }
        return len;
    }
}
