#include "pbonodeevents.h"

namespace pboman3 {
    PboNodeCreatedEvent::PboNodeCreatedEvent(const PboPath* pNodePath, PboNodeType pNodeType)
        : PboNodeEvent(),
          nodePath(pNodePath),
          nodeType(pNodeType) {
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
