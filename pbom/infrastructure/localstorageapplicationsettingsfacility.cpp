#include "localstorageapplicationsettingsfacility.h"
#include "fileconflictresolutionmode.h"
#include "util/enum.h"
#include <QSettings>

#define QSETTINGS(VAR_NAME) QSettings VAR_NAME(QSettings::Format::NativeFormat, QSettings::Scope::UserScope, "pboman3", "settings");
#define KEY_PACK_CONFLICT_RESOLUTION_MODE "pack_conflict_resolution_mode"
#define KEY_UNPACK_CONFLICT_RESOLUTION_MODE "unpack_conflict_resolution_mode"
#define KEY_PACK_UNPACK_OPERATION_COMPLETE_BEHAVIOR "pack_unpack_operation_complete_behavior"

namespace pboman3::infrastructure {
    void LocalStorageApplicationSettingsFacility::purge() {
        QSETTINGS(storage)
        storage.clear();
    }

    QSharedPointer<ApplicationSettings> LocalStorageApplicationSettingsFacility::readSettings() const {
        QSETTINGS(const storage)

        const QVariant vPackConflictResolutionMode = storage.value(KEY_PACK_CONFLICT_RESOLUTION_MODE);
        const QVariant vUnpackConflictResolutionMode = storage.value(KEY_UNPACK_CONFLICT_RESOLUTION_MODE);
        const QVariant vPackUnpackOperationCompleteBehavior = storage.
            value(KEY_PACK_UNPACK_OPERATION_COMPLETE_BEHAVIOR);

        FileConflictResolutionMode::Enum packConflictResolutionMode;
        FileConflictResolutionMode::Enum unpackConflictResolutionMode;
        OperationCompleteBehavior::Enum packUnpackOperationCompleteBehavior;

        if (!FileConflictResolutionMode::tryRead(vPackConflictResolutionMode, packConflictResolutionMode))
            packConflictResolutionMode = FileConflictResolutionMode::Enum::Abort;
        if (!FileConflictResolutionMode::tryRead(vUnpackConflictResolutionMode, unpackConflictResolutionMode))
            unpackConflictResolutionMode = FileConflictResolutionMode::Enum::Abort;
        if (!OperationCompleteBehavior::tryRead(vPackUnpackOperationCompleteBehavior,
                                                packUnpackOperationCompleteBehavior))
            packUnpackOperationCompleteBehavior = OperationCompleteBehavior::Enum::KeepWindow;

        return QSharedPointer<ApplicationSettings>(
            new ApplicationSettings{
                packConflictResolutionMode,
                unpackConflictResolutionMode,
                packUnpackOperationCompleteBehavior
            });
    }

    void LocalStorageApplicationSettingsFacility::writeSettings(const ApplicationSettings* settings) {
        QSETTINGS(storage)

        storage.setValue(
            KEY_PACK_CONFLICT_RESOLUTION_MODE,
            QVariant(static_cast<int>(settings->packConflictResolutionMode)));
        storage.setValue(
            KEY_UNPACK_CONFLICT_RESOLUTION_MODE,
            QVariant(static_cast<int>(settings->unpackConflictResolutionMode)));
        storage.setValue(
            KEY_PACK_UNPACK_OPERATION_COMPLETE_BEHAVIOR,
            QVariant(static_cast<int>(settings->packUnpackOperationCompleteBehavior)));
    }
}
