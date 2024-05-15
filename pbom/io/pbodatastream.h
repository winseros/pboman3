#pragma once

#include "pbofile.h"
#include <QException>

namespace pboman3::io {
    class PboEofException: public QException {    
    };

    template <typename T>
    concept PboHeaderType = std::is_fundamental_v<T> || std::is_enum_v<T>;

    class PboDataStream : public QDataStream {
    public:
        explicit PboDataStream(PboFile* file);

        PboDataStream& operator>>(QString& out);

        PboDataStream& operator<<(const QString& src);

        template <PboHeaderType T>
        PboDataStream& operator>>(T& out) {
            if (file_->read(reinterpret_cast<char*>(&out), sizeof out) != sizeof out) {
                throw PboEofException();
            }
            return *this;
        }

        template <PboHeaderType T>
        PboDataStream& operator<<(const T& src) {
            file_->write(reinterpret_cast<const char*>(&src), sizeof src);
            return *this;
        }

    private:
        PboFile* file_;
    };
}
