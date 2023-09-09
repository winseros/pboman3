#include "compositefilter.h"
#include <algorithm>

namespace pboman3::io {
    CompositeFilter::CompositeFilter(std::initializer_list<QSharedPointer<JunkFilter>> filters)
        : filters_(filters) {
    }

    bool CompositeFilter::isJunk(const PboNodeEntity* entry) const {
        const auto isJunk = std::any_of(filters_.begin(), filters_.end(),
                                        [entry](const QSharedPointer<JunkFilter>& filter) {
                                            return filter->isJunk(entry);
                                        });
        return isJunk;
    }
}
