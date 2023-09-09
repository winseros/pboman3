#pragma once

#include "io/pbonodeentity.h"

namespace pboman3::io {
    class JunkFilter {
    public:
        virtual ~JunkFilter() = default;

        virtual bool isJunk(const PboNodeEntity* entry) const = 0;
    };
}
