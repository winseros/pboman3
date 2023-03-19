#pragma once

#include "junkfilter.h"
#include <QList>
#include <QSharedPointer>

namespace pboman3::io {
    class CompositeFilter : public JunkFilter {
    public:
        explicit CompositeFilter(std::initializer_list<QSharedPointer<JunkFilter>> filters);

        bool IsJunk(const PboNodeEntity* entry) const override;

    private:
        QList<QSharedPointer<JunkFilter>> filters_;
    };
}
