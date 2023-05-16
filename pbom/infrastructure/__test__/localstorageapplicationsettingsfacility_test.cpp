#include <gtest/gtest.h>
#include "../localstorageapplicationsettingsfacility.h"

namespace pboman3::infrastructure::test {
    class LocalStorageApplicationSettingsFacilityTest : public ::testing::Test {
    protected:
        void SetUp() override {
            LocalStorageApplicationSettingsFacility facility;
            facility.purge();
        }

        void TearDown() override {
            LocalStorageApplicationSettingsFacility facility;
            facility.purge();
        }
    };

    TEST_F(LocalStorageApplicationSettingsFacilityTest, ReadSettings_Reads_Values) {
        LocalStorageApplicationSettingsFacility facility;

        constexpr ApplicationSettings savedSettings{
            FileConflictResolutionMode::Enum::Copy,
            FileConflictResolutionMode::Enum::Overwrite,
            OperationCompleteBehavior::Enum::KeepWindow
        };
        facility.writeSettings(&savedSettings);

        const auto readSettings = facility.readSettings();

        ASSERT_EQ(readSettings->packConflictResolutionMode, FileConflictResolutionMode::Enum::Copy);
        ASSERT_EQ(readSettings->unpackConflictResolutionMode, FileConflictResolutionMode::Enum::Overwrite);
        ASSERT_EQ(readSettings->packUnpackOperationCompleteBehavior, OperationCompleteBehavior::Enum::KeepWindow);
    }

    TEST_F(LocalStorageApplicationSettingsFacilityTest, ReadSettings_Reads_Defaults) {
        const LocalStorageApplicationSettingsFacility facility;

        const auto readSettings = facility.readSettings();

        ASSERT_EQ(readSettings->packConflictResolutionMode, FileConflictResolutionMode::Enum::Abort);
        ASSERT_EQ(readSettings->unpackConflictResolutionMode, FileConflictResolutionMode::Enum::Abort);
        ASSERT_EQ(readSettings->packUnpackOperationCompleteBehavior, OperationCompleteBehavior::Enum::KeepWindow);
    }
}
