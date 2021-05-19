#pragma once

#include "pbofile.h"
#include <QException>

namespace pboman3 {
    class PboEofException: public QException {    
    };

    class PboDataStream : public QDataStream {
    public:
        explicit PboDataStream(PboFile* file);

        PboDataStream& operator>>(QString& out);

        PboDataStream& operator<<(const QString& src);

        template <typename T>
        PboDataStream& operator>>(T& out) {
            if (file_->read(reinterpret_cast<char*>(&out), sizeof out) != sizeof out) {
                throw PboEofException();
            }
            return *this;
        }

        template <typename T>
        PboDataStream& operator<<(const T& src) {
            file_->write(reinterpret_cast<const char*>(&src), sizeof src);
            return *this;
        }

    private:
        PboFile* file_;
    };
}
