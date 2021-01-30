#include "pbomodelevents.h"

namespace pboman3 {
    PboModelEvent::PboModelEvent(const QString& eventType)
            : eventType(eventType) {

    }

    const QString PboLoadBeginEvent::eventType = "load-begin";

    PboLoadBeginEvent::PboLoadBeginEvent(const QString& path)
            : PboModelEvent(PboLoadBeginEvent::eventType),
              path(path) {

    }

    const QString PboLoadCompleteEvent::eventType = "load-complete";

    PboLoadCompleteEvent::PboLoadCompleteEvent(const QString& path)
            : PboModelEvent(PboLoadBeginEvent::eventType),
              path(path) {

    }

    const QString PboLoadFailedEvent::eventType = "load-failed";

    PboLoadFailedEvent::PboLoadFailedEvent()
            : PboModelEvent(PboLoadFailedEvent::eventType) {

    }

    const QString PboHeaderUpdatedEvent::eventType = "header-updated";

    PboHeaderUpdatedEvent::PboHeaderUpdatedEvent(const PboHeader* header) :
            PboModelEvent(PboHeaderUpdatedEvent::eventType),
            header(header) {
    }

    const QString PboEntryUpdatedEvent::eventType = "entry-updated";

    PboEntryUpdatedEvent::PboEntryUpdatedEvent(PboEntry* entry)
            : PboModelEvent(PboEntryUpdatedEvent::eventType),
              entry(entry) {
    }
}