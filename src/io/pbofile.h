#pragma once

#include <QFile>

namespace pboman3 {
    class PboFile : public QFile {
    Q_OBJECT
    public:
        PboFile(const QString &name) : QFile(name) {};

        int readCString(QString &value);
    };
}