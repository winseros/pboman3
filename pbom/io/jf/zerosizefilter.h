#pragma once

#include "junkfilter.h"

namespace pboman3::io {
    class ZeroSizeFilter : public JunkFilter {
    public:
        bool IsJunk(const PboNodeEntity* entry) const override;
    };
}
