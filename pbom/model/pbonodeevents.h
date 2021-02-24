#pragma once

#include "pbomodelevents.h"
#include "pbonodetype.h"
#include "pbopath.h"

namespace pboman3 {
    class PboNodeEvent: public PboModelEvent{
    };

    class PboNodeCreatedEvent final : public PboNodeEvent {
    public:
        PboNodeCreatedEvent(const PboPath* pPath, PboNodeType pNodeType);
        const PboPath* path;
        const PboNodeType nodeType;
    };

    class PboNodeMovedEvent final : public PboNodeEvent {
    public:
        PboNodeMovedEvent(const PboPath* pPrevNodePath, const PboPath* pNewNodePath);
        const PboPath* prevNodePath;
        const PboPath* newNodePath;
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