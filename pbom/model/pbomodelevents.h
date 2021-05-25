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

    class PboUnloadEvent final : public PboModelEvent {
    public:
        PboUnloadEvent();
    };

    class PboHeaderCreatedEvent final : public PboModelEvent {
    public:
        const PboHeader* const header;

        explicit PboHeaderCreatedEvent(const PboHeader* header);
    };
}
