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
            std::unique_ptr<char[]> data = std::make_unique<char[]>(len);
            read(data.get(), len);
            seek(pos() + 1);
            value.append(data.get());
        }
        return len;
    }
}
