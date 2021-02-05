#include "pbomodelevents.h"

namespace pboman3 {
    PboModelEvent::PboModelEvent() = default;

    PboLoadBeginEvent::PboLoadBeginEvent(QString path)
        : PboModelEvent(),
          path(std::move(path)) {
    }

    PboLoadCompleteEvent::PboLoadCompleteEvent(QString path)
        : PboModelEvent(),
          path(std::move(path)) {
    }

    PboLoadFailedEvent::PboLoadFailedEvent()
        : PboModelEvent() {
    }

    PboHeaderCreatedEvent::PboHeaderCreatedEvent(const PboHeader* header) :
        PboModelEvent(),
        header(header) {
    }

    PboEntryCreatedEvent::PboEntryCreatedEvent(const PboEntry* entry)
        : PboModelEvent(),
          entry(entry) {
    }

    PboEntryDeleteScheduledEvent::PboEntryDeleteScheduledEvent(const PboEntry* entry)
        : PboModelEvent(),
          entry(entry) {
    }

    PboEntryDeleteCanceledEvent::PboEntryDeleteCanceledEvent(const PboEntry* entry)
        : PboModelEvent(),
          entry(entry) {
    }

    PboEntryDeleteCompleteEvent::PboEntryDeleteCompleteEvent(const PboEntry* entry)
        : PboModelEvent(),
          entry(entry) {
    }


    PboEntryMovedEvent::PboEntryMovedEvent(const PboEntry* prevEntry, const PboEntry* newEntry)
        : PboModelEvent(),
          prevEntry(prevEntry),
          newEntry(newEntry) {
    }
}
