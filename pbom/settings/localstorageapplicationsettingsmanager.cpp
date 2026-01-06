#include "localstorageapplicationsettingsmanager.h"
#include "io/fileconflictresolutionmode.h"
#include <QSettings>

#define QSETTINGS(VAR_NAME) QSettings VAR_NAME(QSettings::Format::NativeFormat, QSettings::Scope::UserScope, "pboman3", "settings");
#define KEY_PACK_CONFLICT_RESOLUTION_MODE "pack_conflict_resolution_mode"
#define KEY_UNPACK_CONFLICT_RESOLUTION_MODE "unpack_conflict_resolution_mode"
#define KEY_PACK_UNPACK_OPERATION_COMPLETE_BEHAVIOR "pack_unpack_operation_complete_behavior"
#define KEY_JUNK_FILTER_ENABLE "junk_filter_enable"
#define KEY_APPLICATION_COLOR_SCHEME "application_color_scheme"

namespace pboman3::settings {
    void LocalStorageApplicationSettingsManager::purge() {
        QSETTINGS(storage)
        storage.clear();
    }

    ApplicationSettings LocalStorageApplicationSettingsManager::readSettings() const {
        QSETTINGS(const storage)

        const QVariant vPackConflictResolutionMode = storage.value(KEY_PACK_CONFLICT_RESOLUTION_MODE);
        const QVariant vUnpackConflictResolutionMode = storage.value(KEY_UNPACK_CONFLICT_RESOLUTION_MODE);
        const QVariant vPackUnpackOperationCompleteBehavior = storage.
            value(KEY_PACK_UNPACK_OPERATION_COMPLETE_BEHAVIOR);
        const QVariant vJunkFilterEnable = storage.value(KEY_JUNK_FILTER_ENABLE);
        const QVariant vApplicationColorScheme = storage.value(KEY_APPLICATION_COLOR_SCHEME);

        io::FileConflictResolutionMode::Enum packConflictResolutionMode;
        io::FileConflictResolutionMode::Enum unpackConflictResolutionMode;
        OperationCompleteBehavior::Enum packUnpackOperationCompleteBehavior;
        ApplicationColorScheme::Enum applicationColorScheme;

        if (!io::FileConflictResolutionMode::tryRead(vPackConflictResolutionMode, packConflictResolutionMode))
            packConflictResolutionMode = io::FileConflictResolutionMode::Enum::Abort;
        if (!io::FileConflictResolutionMode::tryRead(vUnpackConflictResolutionMode, unpackConflictResolutionMode))
            unpackConflictResolutionMode = io::FileConflictResolutionMode::Enum::Abort;
        if (!OperationCompleteBehavior::tryRead(vPackUnpackOperationCompleteBehavior,
                                                packUnpackOperationCompleteBehavior))
            packUnpackOperationCompleteBehavior = OperationCompleteBehavior::Enum::KeepWindow;
        if (!ApplicationColorScheme::tryRead(vApplicationColorScheme, applicationColorScheme))
            applicationColorScheme = ApplicationColorScheme::Enum::Auto;

        return ApplicationSettings{
            packConflictResolutionMode,
            unpackConflictResolutionMode,
            packUnpackOperationCompleteBehavior,
            vJunkFilterEnable.isNull() ? true : vJunkFilterEnable.toBool(),
            applicationColorScheme
        };
    }

    void LocalStorageApplicationSettingsManager::writeSettings(const ApplicationSettings& settings) {
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
        storage.setValue(
            KEY_APPLICATION_COLOR_SCHEME,
            QVariant(static_cast<int>(settings.applicationColorScheme)));

        emit settingsChanged(settings);
    }
}
