#include "pbonodeevents.h"

namespace pboman3 {
    PboNodeCreatedEvent::PboNodeCreatedEvent(const PboPath* pNodePath, PboNodeType pNodeType)
        : PboNodeEvent(),
          nodePath(pNodePath),
          nodeType(pNodeType) {
    }

    PboNodeMovedEvent::PboNodeMovedEvent(const PboPath* pPrevNodePath, const PboPath* pNewNodePath)
        : PboNodeEvent(),
          prevNodePath(pPrevNodePath),
          newNodePath(pNewNodePath) {
    }

    PboNodeRenamedEvent::PboNodeRenamedEvent(const PboPath* pNodePath, QString pNewNodeTitle)
        : PboNodeEvent(),
          nodePath(pNodePath),
          newNodeTitle(std::move(pNewNodeTitle)) {
    }

    PboNodeRemovedEvent::PboNodeRemovedEvent(const PboPath* pNodePath)
        : PboNodeEvent(),
          nodePath(pNodePath) {
    }
}
