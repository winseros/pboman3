#include "io/jf/compositefilter.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace pboman3::io::test {
    class MockFilter : public JunkFilter {
    public:
        MOCK_METHOD(bool, IsJunk, (const PboNodeEntity* entry), (const, override));
    };

    TEST(CompositeFilterTest, IsJunk_Returns_False) {
        testing::NiceMock<MockFilter> mock1;
        testing::NiceMock<MockFilter> mock2;

        const PboNodeEntity entry{"fn", PboPackingMethod::Uncompressed, 1, 0, 2, 3};
        const CompositeFilter filter({
            QSharedPointer<JunkFilter>(&mock1, [](void*) {
            }),
            QSharedPointer<JunkFilter>(&mock2, [](void*) {
            })
        });

        EXPECT_CALL(mock1, IsJunk(&entry))
        .WillOnce(testing::Return(false));

        EXPECT_CALL(mock2, IsJunk(&entry))
        .WillOnce(testing::Return(false));

        const auto isJunk = filter.IsJunk(&entry);
        ASSERT_FALSE(isJunk);
    }

    TEST(CompositeTestFilter, IsJunk_Returns_True) {
        testing::NiceMock<MockFilter> mock1;
        testing::NiceMock<MockFilter> mock2;

        const PboNodeEntity entry{"fn", PboPackingMethod::Uncompressed, 1, 0, 2, 3};
        const CompositeFilter filter({
            QSharedPointer<JunkFilter>(&mock1, [](void*) {
            }),
            QSharedPointer<JunkFilter>(&mock2, [](void*) {
            })
        });

        EXPECT_CALL(mock1, IsJunk(&entry))
        .WillOnce(testing::Return(true));

        EXPECT_CALL(mock2, IsJunk(testing::_))
        .Times(0);

        const auto isJunk = filter.IsJunk(&entry);
        ASSERT_TRUE(isJunk);
    }
}
