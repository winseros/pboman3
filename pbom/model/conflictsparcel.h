#pragma once

#include "conflictresolution.h"
#include "interactionparcel.h"
#include <QHash>

namespace pboman3 {
    class ConflictsParcel {
    public:
        ConflictResolution getResolution(const NodeDescriptor& descriptor) const;

        void setResolution(const NodeDescriptor& descriptor, ConflictResolution resolution);

        bool hasConflicts() const;
    private:
        QHash<PboPath, ConflictResolution> conflicts_;
    };
}
