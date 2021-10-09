#include "fslzhbinarysource.h"
#include "io/lzh/lzh.h"

namespace pboman3 {
    FsLzhBinarySource::FsLzhBinarySource(QString path, qsizetype bufferSize)
        : FsRawBinarySource(std::move(path), bufferSize){
    }

    void FsLzhBinarySource::writeToPbo(QFileDevice* targetFile, const Cancel& cancel) {
        assert(file_->isOpen());
        const bool seek = file_->seek(0);
        assert(seek);
        Lzh::compress(file_, targetFile, cancel);
    }

    bool FsLzhBinarySource::isCompressed() const {
        return true;
    }
}
