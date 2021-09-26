#pragma once

#include "pbonode.h"
#include "io/pboheaderreader.h"

namespace pboman3 {
    class RootReader {
    public:
        RootReader(const PboFileHeader* header, const QString& path);

        void inflateRoot(PboNode* root) const;

    private:
        const PboFileHeader* header_;
        const QString& path_;
    };
}
