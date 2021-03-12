#include "io/lzh.h"
#include <QTemporaryFile>
#include <gtest/gtest.h>

namespace pboman3::test {
    struct LzhTestParam {
        QString original;
        QString source;
    };

    class LzhTest : public testing::TestWithParam<LzhTestParam> {
    };

    TEST_P(LzhTest, TryDecompress_Unpacks_Lzh) {
        QTemporaryFile t;
        t.open();

        const LzhTestParam p = GetParam();

        QFile original(p.original);
        original.open(QIODeviceBase::ReadOnly);
        assert(original.size() && "Could not open the file for some reason");

        QFile source(p.source);
        source.open(QIODeviceBase::ReadOnly);

        Lzh::decompress(&source, &t, original.size(), []() { return false; });

        t.seek(0);
        const QByteArray targetBytes = t.readAll();
        const QByteArray originalBytes = original.readAll();

        t.close();
        original.close();
        source.close();

        ASSERT_TRUE(targetBytes.length());
        ASSERT_EQ(originalBytes.length(), targetBytes.length());
        ASSERT_EQ(originalBytes, targetBytes);
    }

#define TEST_FILE(F) SOURCE_DIR F
    INSTANTIATE_TEST_SUITE_P(LzhTest, LzhTest,
                             testing::Values(
                                 LzhTestParam{
                                 TEST_FILE("\\io\\__test__\\data\\lzh\\gpl-3.0.txt") ,
                                 TEST_FILE("\\io\\__test__\\data\\lzh\\gpl-3.0.lzh") },
                                 LzhTestParam{
                                 TEST_FILE("\\io\\__test__\\data\\lzh\\mission.sqm") ,
                                 TEST_FILE("\\io\\__test__\\data\\lzh\\mission.lzh") }
                             ));
}
