#include "pbodatastream.h"

namespace pboman3 {
    PboDataStream::PboDataStream(PboFile* file)
        : QDataStream(file),
          file_(file) {
    }

    PboDataStream& PboDataStream::operator>>(QString& out) {
        file_->readCString(out);
        return *this;
    }

    PboDataStream& PboDataStream::operator<<(const QString& src) {
        file_->writeCString(src);
        return *this;
    }

}
