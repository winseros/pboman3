#include "pbomodelevents.h"

namespace pboman3 {
    PboModelEvent::PboModelEvent(QString eventType)
        : eventType(std::move(eventType)) {
    }

    const QString PboLoadBeginEvent::eventType = "load-begin";

    PboLoadBeginEvent::PboLoadBeginEvent(QString path)
        : PboModelEvent(PboLoadBeginEvent::eventType),
          path(std::move(path)) {
    }

    const QString PboLoadCompleteEvent::eventType = "load-complete";

    PboLoadCompleteEvent::PboLoadCompleteEvent(QString path)
        : PboModelEvent(PboLoadCompleteEvent::eventType),
          path(std::move(path)) {
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

    const QString PboEntryMovedEvent::eventType = "entry-moved";

    PboEntryMovedEvent::PboEntryMovedEvent(const PboEntry* prevEntry, const PboEntry* newEntry)
        : PboModelEvent(PboEntryMovedEvent::eventType),
          prevEntry(prevEntry),
          newEntry(newEntry) {
    }
}
