#include "localstorageapplicationsettingsfacility.h"
#include "fileconflictresolutionmode.h"
#include <QSettings>

#define QSETTINGS(VAR_NAME) QSettings VAR_NAME(QSettings::Format::NativeFormat, QSettings::Scope::UserScope, "pboman3", "settings");
#define KEY_PACK_CONFLICT_RESOLUTION_MODE "pack_conflict_resolution_mode"
#define KEY_UNPACK_CONFLICT_RESOLUTION_MODE "unpack_conflict_resolution_mode"
#define KEY_PACK_UNPACK_OPERATION_COMPLETE_BEHAVIOR "pack_unpack_operation_complete_behavior"
#define KEY_JUNK_FILTER_ENABLE "junk_filter_enable"

namespace pboman3::io {
    void LocalStorageApplicationSettingsFacility::purge() {
        QSETTINGS(storage)
        storage.clear();
    }

    ApplicationSettings LocalStorageApplicationSettingsFacility::readSettings() const {
        QSETTINGS(const storage)

        const QVariant vPackConflictResolutionMode = storage.value(KEY_PACK_CONFLICT_RESOLUTION_MODE);
        const QVariant vUnpackConflictResolutionMode = storage.value(KEY_UNPACK_CONFLICT_RESOLUTION_MODE);
        const QVariant vPackUnpackOperationCompleteBehavior = storage.
            value(KEY_PACK_UNPACK_OPERATION_COMPLETE_BEHAVIOR);
        const QVariant vJunkFilterEnable = storage.value(KEY_JUNK_FILTER_ENABLE);

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

        return ApplicationSettings{
            packConflictResolutionMode,
            unpackConflictResolutionMode,
            packUnpackOperationCompleteBehavior,
            vJunkFilterEnable.isNull() ? true : vJunkFilterEnable.toBool()
        };
    }

    void LocalStorageApplicationSettingsFacility::writeSettings(const ApplicationSettings& settings) {
        QSETTINGS(storage)

        storage.setValue(
            KEY_PACK_CONFLICT_RESOLUTION_MODE,
            QVariant(static_cast<int>(settings.packConflictResolutionMode)));
        storage.setValue(
            KEY_UNPACK_CONFLICT_RESOLUTION_MODE,
            QVariant(static_cast<int>(settings.unpackConflictResolutionMode)));
        storage.setValue(
            KEY_PACK_UNPACK_OPERATION_COMPLETE_BEHAVIOR,
            QVariant(static_cast<int>(settings.packUnpackOperationCompleteBehavior)));
        storage.setValue(KEY_JUNK_FILTER_ENABLE, QVariant(settings.junkFilterEnable));
    }
}
