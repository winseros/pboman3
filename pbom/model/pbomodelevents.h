#pragma once

#include "pboentry.h"
#include "pboheader.h"

namespace pboman3 {
    class PboModelEvent {
    protected:
        explicit PboModelEvent();

        virtual ~PboModelEvent() = default;
    };

    class PboLoadBeginEvent final : public PboModelEvent {
    public:
        const QString path;

        explicit PboLoadBeginEvent(QString path);
    };

    class PboLoadCompleteEvent final : public PboModelEvent {
    public:
        const QString path;

        explicit PboLoadCompleteEvent(QString path);
    };

    class PboLoadFailedEvent final : public PboModelEvent {
    public:
        PboLoadFailedEvent();
    };

    class PboHeaderCreatedEvent final : public PboModelEvent {
    public:
        const PboHeader* const header;

        explicit PboHeaderCreatedEvent(const PboHeader* header);
    };

    class PboEntryCreatedEvent final : public PboModelEvent {
    public:
        const PboEntry* entry;

        explicit PboEntryCreatedEvent(const PboEntry* entry);
    };

    class PboEntryDeleteScheduledEvent final : public PboModelEvent {
    public:
        const PboEntry* entry;

        explicit PboEntryDeleteScheduledEvent(const PboEntry* entry);
    };

    class PboEntryDeleteCanceledEvent final : public PboModelEvent {
    public:
        const PboEntry* entry;

        explicit PboEntryDeleteCanceledEvent(const PboEntry* entry);
    };

    class PboEntryDeleteCompleteEvent final : public PboModelEvent {
    public:
        const PboEntry* entry;

        explicit PboEntryDeleteCompleteEvent(const PboEntry* entry);
    };

    class PboEntryMoveScheduledEvent final : public PboModelEvent {
    public:
        const PboEntry* entry;
        const PboEntry* movedEntry;

        PboEntryMoveScheduledEvent(const PboEntry* entry, const PboEntry* movedEntry);
    };

    class PboEntryMoveCanceledEvent final : public PboModelEvent {
    public:
        const PboEntry* entry;
        const PboEntry* movedEntry;

        PboEntryMoveCanceledEvent(const PboEntry* entry, const PboEntry* movedEntry);
    };

    class PboEntryMoveCompleteEvent final : public PboModelEvent {
    public:
        const PboEntry* entry;
        const PboEntry* movedEntry;

        PboEntryMoveCompleteEvent(const PboEntry* entry, const PboEntry* movedEntry);
    };
}
