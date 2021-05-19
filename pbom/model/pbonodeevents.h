#pragma once

#include "pbomodelevents.h"
#include "pbonodetype.h"
#include "pbopath.h"

namespace pboman3 {
    class PboNodeEvent: public PboModelEvent{
    };

    class PboNodeCreatedEvent final : public PboNodeEvent {
    public:
        PboNodeCreatedEvent(const PboPath* pNodePath, PboNodeType pNodeType);
        const PboPath* nodePath;
        const PboNodeType nodeType;
    };

    class PboNodeRenamedEvent final : public PboNodeEvent {
    public:
        PboNodeRenamedEvent(const PboPath* pNodePath, QString pNewNodeTitle);
        const PboPath* nodePath;
        const QString newNodeTitle;
    };

    class PboNodeRemovedEvent final : public PboNodeEvent {
    public:
        PboNodeRemovedEvent(const PboPath* pNodePath);
        const PboPath* nodePath;
    };
}
