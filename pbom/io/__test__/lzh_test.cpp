#include "io/lzh.h"
#include <QTemporaryFile>
#include <gtest/gtest.h>

namespace pboman3::test {
#define TEST_FILE(F) SOURCE_DIR F

    TEST(LzhTest, TryDecompress_Unpacks_Lzh) {
        QTemporaryFile t;
        t.open();

        QFile original(TEST_FILE("\\io\\__test__\\data\\lzh\\gpl-3.0.txt"));
        original.open(QIODeviceBase::ReadOnly);
        assert(original.size() && "Could not open the file for some reason");

        QFile source(TEST_FILE("\\io\\__test__\\data\\lzh\\gpl-3.0.lzh"));
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
}
