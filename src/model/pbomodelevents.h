#pragma once

#include <QString>
#include "pboentry.h"
#include "pboheader.h"

namespace pboman3 {
    class PboModelEvent {
    public:
        const QString eventType;
    protected:
        explicit PboModelEvent(const QString& eventType);

        virtual ~PboModelEvent() = default;
    };

    class PboLoadBeginEvent final : public virtual PboModelEvent {
    public:
        static const QString eventType;

        const QString path;

        explicit PboLoadBeginEvent(const QString& path);
    };

    class PboLoadCompleteEvent final : public virtual PboModelEvent {
    public:
        static const QString eventType;

        const QString path;

        explicit PboLoadCompleteEvent(const QString& path);
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
}