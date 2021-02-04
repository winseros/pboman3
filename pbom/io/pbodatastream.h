#pragma once

#include "pbofile.h"

namespace pboman3 {
    class PboDataStream : public QDataStream {
    public:
        explicit PboDataStream(PboFile* file);

        PboDataStream& operator>>(QString& out);

        PboDataStream& operator<<(const QString& src);

        template <typename T>
        PboDataStream& operator>>(T& out) {
            file_->read(reinterpret_cast<char*>(&out), sizeof out);
            return *this;
        }

        template <typename T>
        PboDataStream& operator<<(T& src) {
            file_->write(reinterpret_cast<const char*>(&src), sizeof src);
            return *this;
        }

    private:
        PboFile* file_;
    };
}
