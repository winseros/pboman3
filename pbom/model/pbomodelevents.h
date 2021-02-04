#pragma once

#include "pboentry.h"
#include "pboheader.h"

namespace pboman3 {
    class PboModelEvent {
    public:
        const QString eventType;
    protected:
        explicit PboModelEvent(QString eventType);

        virtual ~PboModelEvent() = default;
    };

    class PboLoadBeginEvent final : public virtual PboModelEvent {
    public:
        static const QString eventType;

        const QString path;

        explicit PboLoadBeginEvent(QString path);
    };

    class PboLoadCompleteEvent final : public virtual PboModelEvent {
    public:
        static const QString eventType;

        const QString path;

        explicit PboLoadCompleteEvent(QString path);
    };

    class PboLoadFailedEvent final : public virtual PboModelEvent {
    public:
        static const QString eventType;

        PboLoadFailedEvent();
    };

    class PboHeaderUpdatedEvent final : public virtual PboModelEvent {
    public:
        static const QString eventType;

        const PboHeader* const header;

        explicit PboHeaderUpdatedEvent(const PboHeader* header);
    };

    class PboEntryUpdatedEvent final : public virtual PboModelEvent {
    public:
        static const QString eventType;

        const PboEntry* entry;

        explicit PboEntryUpdatedEvent(PboEntry* entry);
    };

    class PboEntryMovedEvent final : public virtual PboModelEvent {
    public:
        static const QString eventType;

        const PboEntry* prevEntry;
        const PboEntry* newEntry;

        PboEntryMovedEvent(const PboEntry* prevEntry, const PboEntry* newEntry);
    };
}
