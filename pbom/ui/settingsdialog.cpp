#include "settingsdialog.h"
#include "util/log.h"
#include "infrastructure/localstorageapplicationsettingsfacility.h"
#include "infrastructure/fileconflictresolutionmode.h"

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
        using namespace infrastructure;

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
        facility.writeSettings(&settings);
    }

    void SettingsDialog::loadSettings() const {
        using namespace infrastructure;
        const LocalStorageApplicationSettingsFacility facility;
        const auto settings = facility.readSettings();

        setRadioButtonValue(ui_->groupBoxPack, settings->packConflictResolutionMode);
        setRadioButtonValue(ui_->groupBoxUnpack, settings->packConflictResolutionMode);
        setRadioButtonValue(ui_->groupBoxOpComplete, settings->packUnpackOperationCompleteBehavior);
    }
}
