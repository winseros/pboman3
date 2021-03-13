#include "io/lzh/lzh.h"
#include <QTemporaryFile>
#include <gtest/gtest.h>

namespace pboman3::test {
    struct LzhTestParam {
        QString original;
        QString source;
    };

    class DecompressTest : public testing::TestWithParam<LzhTestParam> {
    };

    TEST_P(DecompressTest, Decompress_Unpacks_Lzh) {
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

        ASSERT_TRUE(targetBytes.length());
        ASSERT_EQ(originalBytes.length(), targetBytes.length());
        ASSERT_EQ(originalBytes, targetBytes);
    }

    TEST(LzhTest, Decompress_Cancels) {
        QTemporaryFile t;
        t.open();

        QTemporaryFile t2;
        t2.open();
        t2.write(QByteArray("some compressed data normally be here"));
        t2.seek(0);

        int cancelCounter = 0;
        const Cancel cancel = [&cancelCounter]() {
            cancelCounter++;
            return cancelCounter > 1;
        };
        Lzh::decompress(&t2, &t, 100, cancel);

        t.seek(0);
        const QByteArray targetBytes = t.readAll();

        ASSERT_EQ(cancelCounter, 3);//1 - the 1st check in cycle, 2 - the 2nd check, 3 - check when validating
        ASSERT_EQ(targetBytes.length(), 32);
    }

    class CompressTest : public testing::TestWithParam<LzhTestParam> {
    };

    TEST_P(CompressTest, Compress_Packs_Lzh) {
        QTemporaryFile t;
        t.open();

        const LzhTestParam p = GetParam();

        QFile original(p.original);
        original.open(QIODeviceBase::ReadOnly);
        assert(original.size() && "Could not open the file for some reason");

        QFile source(p.source);
        source.open(QIODeviceBase::ReadOnly);

        Lzh::compress(&source, &t, []() { return false; });

        t.seek(0);
        const QByteArray targetBytes = t.readAll();
        const QByteArray originalBytes = original.readAll();

        ASSERT_TRUE(targetBytes.length());
        ASSERT_EQ(originalBytes.length(), targetBytes.length());
        ASSERT_EQ(originalBytes, targetBytes);
    }

    TEST(LzhTest, Compress_Cancels) {
        QTemporaryFile t;
        t.open();

        QTemporaryFile t2;
        t2.open();
        t2.write(QByteArray("some data normally is here"));
        t2.seek(0);

        int cancelCounter = 0;
        const Cancel cancel = [&cancelCounter]() {
            cancelCounter++;
            return cancelCounter > 1;
        };
        Lzh::compress(&t2, &t, cancel);

        t.seek(0);
        const QByteArray targetBytes = t.readAll();

        ASSERT_EQ(cancelCounter, 3);//1 - the 1st check in cycle, 2 - the 2nd check, 3 - crc check
        ASSERT_EQ(targetBytes.length(), 9);
    }

#define TEST_FILE(F) SOURCE_DIR F
    INSTANTIATE_TEST_SUITE_P(LzhTest, DecompressTest,
                             testing::Values(
                                 LzhTestParam{
                                 TEST_FILE("\\io\\lzh\\__test__\\data\\lzh\\gpl-3.0.txt") ,
                                 TEST_FILE("\\io\\lzh\\__test__\\data\\lzh\\gpl-3.0.lzh") },
                                 LzhTestParam{
                                 TEST_FILE("\\io\\lzh\\__test__\\data\\lzh\\mission.sqm") ,
                                 TEST_FILE("\\io\\lzh\\__test__\\data\\lzh\\mission.lzh") }
                             ));

    INSTANTIATE_TEST_SUITE_P(LzhTest, CompressTest,
                             testing::Values(
                                 LzhTestParam{
                                 TEST_FILE("\\io\\lzh\\__test__\\data\\lzh\\gpl-3.0.lzh") ,
                                 TEST_FILE("\\io\\lzh\\__test__\\data\\lzh\\gpl-3.0.txt") },
                                 LzhTestParam{
                                 TEST_FILE("\\io\\lzh\\__test__\\data\\lzh\\mission.lzh") ,
                                 TEST_FILE("\\io\\lzh\\__test__\\data\\lzh\\mission.sqm") }
                             ));
}
