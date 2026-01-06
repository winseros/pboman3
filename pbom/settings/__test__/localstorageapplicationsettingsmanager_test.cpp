#include <gtest/gtest.h>
#include "../localstorageapplicationsettingsmanager.h"

namespace pboman3::settings::test {
    class LocalStorageApplicationSettingsManagerTest : public ::testing::Test {
    protected:
        void SetUp() override {
            LocalStorageApplicationSettingsManager::purge();
        }

        void TearDown() override {
            LocalStorageApplicationSettingsManager::purge();
        }
    };

    TEST_F(LocalStorageApplicationSettingsManagerTest, ReadSettings_Reads_Values) {
        LocalStorageApplicationSettingsManager facility;

        constexpr ApplicationSettings savedSettings{
            io::FileConflictResolutionMode::Enum::Copy,
            io::FileConflictResolutionMode::Enum::Overwrite,
            OperationCompleteBehavior::Enum::KeepWindow
        };
        facility.writeSettings(savedSettings);

        const auto readSettings = facility.readSettings();

        ASSERT_EQ(readSettings.packConflictResolutionMode, io::FileConflictResolutionMode::Enum::Copy);
        ASSERT_EQ(readSettings.unpackConflictResolutionMode, io::FileConflictResolutionMode::Enum::Overwrite);
        ASSERT_EQ(readSettings.packUnpackOperationCompleteBehavior, OperationCompleteBehavior::Enum::KeepWindow);
    }

    TEST_F(LocalStorageApplicationSettingsManagerTest, ReadSettings_Reads_Defaults) {
        const LocalStorageApplicationSettingsManager facility;

        const auto readSettings = facility.readSettings();

        ASSERT_EQ(readSettings.packConflictResolutionMode, io::FileConflictResolutionMode::Enum::Abort);
        ASSERT_EQ(readSettings.unpackConflictResolutionMode, io::FileConflictResolutionMode::Enum::Abort);
        ASSERT_EQ(readSettings.packUnpackOperationCompleteBehavior, OperationCompleteBehavior::Enum::KeepWindow);
    }
}
