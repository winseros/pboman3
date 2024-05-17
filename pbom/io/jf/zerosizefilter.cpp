#include "zerosizefilter.h"

namespace pboman3::io {
    bool ZeroSizeFilter::isJunk(const PboNodeEntity* entry) const {
        return entry->dataSize() <= 0 || entry->packingMethod() == PboPackingMethod::Packed && entry->originalSize() <= 0;
    }
}
