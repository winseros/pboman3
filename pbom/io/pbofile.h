#pragma once

#include <QFile>

namespace pboman3::io {
    class PboFile : public QFile {
    Q_OBJECT
    public:
        explicit PboFile(const QString& name);

        int readCString(QString& value);

        int writeCString(const QString& value);
    };
}
