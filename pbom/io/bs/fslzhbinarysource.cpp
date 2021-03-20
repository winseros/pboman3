#include "fslzhbinarysource.h"
#include "io/lzh/lzh.h"

namespace pboman3 {
    FsLzhBinarySource::FsLzhBinarySource(const QString& path, size_t bufferSize)
        : FsRawBinarySource(path, bufferSize){
    }

    void FsLzhBinarySource::writeToPbo(QFileDevice* targetFile, const Cancel& cancel) {
        file_->seek(0);
        Lzh::compress(file_, targetFile, cancel);
    }
}
