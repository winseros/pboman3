#pragma once

#include "pbofile.h"

namespace pboman3 {
    class PboDataStream : public QDataStream {
    public:
        explicit PboDataStream(PboFile* file);

        PboDataStream& operator>>(QString& out);

        template<typename T>
        inline PboDataStream& operator>>(T& out) {
            file_->read(reinterpret_cast<char*>(&out), sizeof(out));
            return *this;
        }

    private:
        PboFile* file_;
    };
}


