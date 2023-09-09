#include "dummyfilter.h"


namespace pboman3::io {
    bool DummyFilter::isJunk(const PboNodeEntity* entry) const {
        return false;
    }
}
