#include "conflictsparcel.h"

namespace pboman3::model {
    ConflictResolution ConflictsParcel::getResolution(const NodeDescriptor& descriptor) const {
        return conflicts_.contains(descriptor.path()) ? conflicts_[descriptor.path()] : ConflictResolution::Unset;
    }

    void ConflictsParcel::setResolution(const NodeDescriptor& descriptor, ConflictResolution resolution) {
        conflicts_[descriptor.path()] = resolution;
    }

    bool ConflictsParcel::hasConflicts() const {
        return conflicts_.count() > 0;
    }

    QDebug operator<<(QDebug debug, const ConflictsParcel& parcel) {
        return debug << "ConflictsParcel(" << parcel.conflicts_.count() << "items)";
    }
}
