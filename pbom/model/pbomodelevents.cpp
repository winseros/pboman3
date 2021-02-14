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


    PboEntryMoveScheduledEvent::PboEntryMoveScheduledEvent(const PboEntry* entry, const PboEntry* movedEntry)
        : PboModelEvent(),
          entry(entry),
          movedEntry(movedEntry) {
    }

    PboEntryMoveCanceledEvent::PboEntryMoveCanceledEvent(const PboEntry* entry, const PboEntry* movedEntry)
        : PboModelEvent(),
          entry(entry),
          movedEntry(movedEntry) {
    }

    PboEntryMoveCompleteEvent::PboEntryMoveCompleteEvent(const PboEntry* entry, const PboEntry* movedEntry)
        : PboModelEvent(),
          entry(entry),
          movedEntry(movedEntry) {
    }
}
