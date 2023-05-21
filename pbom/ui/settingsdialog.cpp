#include "settingsdialog.h"
#include "util/log.h"
#include "io/settings/localstorageapplicationsettingsfacility.h"
#include "io/settings/fileconflictresolutionmode.h"

#define LOG(...) LOGGER("ui/SettingsDialog", __VA_ARGS__)

namespace pboman3::ui {
    SettingsDialog::SettingsDialog(QWidget* parent)
        : QDialog(parent),
          ui_(new Ui::SettingsDialog) {
        ui_->setupUi(this);

        loadSettings();
    }

    SettingsDialog::~SettingsDialog() {
        delete ui_;
    }

    void SettingsDialog::accept() {
        using namespace io;

        const auto packConflictResolutionMode = getRadioButtonValue<
            FileConflictResolutionMode::Enum>(ui_->groupBoxPack);
        const auto unpackConflictResolutionMode = getRadioButtonValue<FileConflictResolutionMode::Enum>(
            ui_->groupBoxUnpack);
        const auto packUnpackOperationCompleteBehavior = getRadioButtonValue<OperationCompleteBehavior::Enum>(
            ui_->groupBoxOpComplete);

        const ApplicationSettings settings{
            packConflictResolutionMode,
            unpackConflictResolutionMode,
            packUnpackOperationCompleteBehavior
        };
        LocalStorageApplicationSettingsFacility facility;
        facility.writeSettings(settings);

        close();
    }

    void SettingsDialog::loadSettings() const {
        using namespace io;
        const LocalStorageApplicationSettingsFacility facility;
        const auto settings = facility.readSettings();

        setRadioButtonValue(ui_->groupBoxPack, settings.packConflictResolutionMode);
        setRadioButtonValue(ui_->groupBoxUnpack, settings.unpackConflictResolutionMode);
        setRadioButtonValue(ui_->groupBoxOpComplete, settings.packUnpackOperationCompleteBehavior);
    }
}
