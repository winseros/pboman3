#include "settingsdialog.h"
#include "util/log.h"
#include "settings/getapplicationsettingsmanager.h"
#include "io/fileconflictresolutionmode.h"

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
        using namespace settings;

        const auto packConflictResolutionMode = getRadioButtonValue<
            io::FileConflictResolutionMode::Enum>(ui_->groupBoxPack);
        const auto unpackConflictResolutionMode = getRadioButtonValue<io::FileConflictResolutionMode::Enum>(
            ui_->groupBoxUnpack);
        const auto packUnpackOperationCompleteBehavior = getRadioButtonValue<OperationCompleteBehavior::Enum>(
            ui_->groupBoxOpComplete);
        const auto filterJunkFiles = ui_->cbJunkFilterEnable->isChecked();
        const auto colorScheme = getRadioButtonValue<ApplicationColorScheme::Enum>(ui_->groupBoxColorScheme);

        const ApplicationSettings settings{
            packConflictResolutionMode,
            unpackConflictResolutionMode,
            packUnpackOperationCompleteBehavior,
            filterJunkFiles,
            colorScheme
        };

        GetApplicationSettingsManager()->writeSettings(settings);

        close();
    }

    void SettingsDialog::loadSettings() const {
        using namespace settings;

        const auto settings = GetApplicationSettingsManager()->readSettings();

        setRadioButtonValue(ui_->groupBoxPack, settings.packConflictResolutionMode);
        setRadioButtonValue(ui_->groupBoxUnpack, settings.unpackConflictResolutionMode);
        setRadioButtonValue(ui_->groupBoxOpComplete, settings.packUnpackOperationCompleteBehavior);
        ui_->cbJunkFilterEnable->setChecked(settings.junkFilterEnable);
        setRadioButtonValue(ui_->groupBoxColorScheme, settings.applicationColorScheme);
    }
}
