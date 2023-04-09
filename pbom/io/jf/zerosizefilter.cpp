#include "zerosizefilter.h"

namespace pboman3::io {
    bool ZeroSizeFilter::IsJunk(const PboNodeEntity* entry) const {
        return entry->dataSize() <= 0;
    }
}
