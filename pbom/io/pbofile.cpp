#include "pbofile.h"

namespace pboman3::io {
    PboFile::PboFile(const QString& name)
        : QFile(name) {
    }

    int PboFile::readCString(QString& value) {
        bool found = false;
        int len = 0;
        startTransaction();
        while (!atEnd()) {
            char data;
            if (read(&data, sizeof data) && data == 0) {
                found = true;
                break;
            }
            len++;
        }
        rollbackTransaction();

        if (found) {
            const auto initialPos = pos();
            if (len) {
                QByteArray bytes(len, Qt::Initialization::Uninitialized);
                read(bytes.data(), len);
                value.append(bytes);
            }
            seek(pos() + 1);
            return static_cast<int>(pos() - initialPos);
        }
        return 0;
    }

    int PboFile::writeCString(const QString& value) {
        write(value.toUtf8());
        constexpr char zero = 0;
        write(&zero, sizeof zero);
        return static_cast<int>(value.length() + sizeof zero);
    }
}
