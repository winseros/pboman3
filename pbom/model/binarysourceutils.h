#pragma once

#include "exception.h"
#include "domain/binarysource.h"
#include "io/bs/pbobinarysource.h"
#include "io/bs/fsrawbinarysource.h"
#include "io/bs/fslzhbinarysource.h"

namespace pboman3::model {
    using namespace domain;
    using namespace io;

    inline void ChangeBinarySourceCompressionMode(QSharedPointer<BinarySource>& bs, bool compress) {
        if (dynamic_cast<PboBinarySource*>(bs.get())) {
            throw InvalidOperationException("Can't query compression status");
        }

        if (compress) {
            if (dynamic_cast<FsRawBinarySource*>(bs.get())) {
                bs = QSharedPointer<BinarySource>(new FsLzhBinarySource(bs->path()));
                bs->open();
            }
        } else {
            if (dynamic_cast<FsLzhBinarySource*>(bs.get())) {
                bs = QSharedPointer<BinarySource>(new FsRawBinarySource(bs->path()));
                bs->open();
            }
        }
    }

    inline bool IsCompressed(const QSharedPointer<BinarySource>& bs) {
        if (const auto pboBs = dynamic_cast<PboBinarySource*>(bs.get())) {
            return pboBs->isCompressed();
        }
        throw InvalidOperationException("Can't query compression status");
    }
}
