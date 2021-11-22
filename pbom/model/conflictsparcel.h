#pragma once

#include "domain/conflictresolution.h"
#include "domain/pbopath.h"
#include "interactionparcel.h"
#include <QHash>

namespace pboman3::model {
    class ConflictsParcel {
    public:
        ConflictResolution getResolution(const NodeDescriptor& descriptor) const;

        void setResolution(const NodeDescriptor& descriptor, ConflictResolution resolution);

        bool hasConflicts() const;

        friend QDebug operator<<(QDebug debug, const ConflictsParcel& parcel);

    private:
        QHash<PboPath, ConflictResolution> conflicts_;
    };
}
