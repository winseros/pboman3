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

    PboUnloadEvent::PboUnloadEvent()
        : PboModelEvent() {
    }

    PboHeaderCreatedEvent::PboHeaderCreatedEvent(const PboHeader* header) :
        PboModelEvent(),
        header(header) {
    }
}
