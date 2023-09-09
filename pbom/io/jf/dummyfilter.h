#pragma once

#include "junkfilter.h"

namespace pboman3::io {
    class DummyFilter : public JunkFilter {
    public:
        bool isJunk(const PboNodeEntity* entry) const override;
    };
}
