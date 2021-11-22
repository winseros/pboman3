#include "io/lzh/compressionchunk.h"
#include <QTemporaryFile>
#include <gtest/gtest.h>

namespace pboman3::io::test {
    TEST(CompressionChunkTest, Compose_Fulfills_Packet) {
        QByteArray dummy;
        dummy.resize(100);
        dummy.fill(0, dummy.size());

        QTemporaryFile t1;
        t1.open();
        t1.write(dummy);
        t1.seek(0);

        CompressionBuffer dict;
        CompressionChunk chunk;

        const qint64 processed = chunk.compose(&t1, dict);

        ASSERT_EQ(processed, 100);
        ASSERT_EQ(t1.pos(), 100);
    }

    TEST(CompressionChunkTest, Compose_Fills_Packet_Until_Source_Has_Data) {
        QByteArray dummy;
        dummy.resize(50);
        dummy.fill(0, dummy.size());

        QTemporaryFile t1;
        t1.open();
        t1.write(dummy);
        t1.seek(0);

        CompressionBuffer dict;
        CompressionChunk chunk;

        const qint64 processed = chunk.compose(&t1, dict);

        ASSERT_EQ(processed, 50);
        ASSERT_TRUE(t1.atEnd());
    }

    TEST(CompressionChunkTest, Flush_Writes_Chunk) {
        QByteArray dummy;
        dummy.resize(50);
        dummy.fill(0, dummy.size());

        QTemporaryFile t1;
        t1.open();
        t1.write(dummy);
        t1.seek(0);

        QTemporaryFile t2;
        t2.open();

        CompressionBuffer dict;
        CompressionChunk chunk;

        chunk.compose(&t1, dict);
        const qint64 written = chunk.flush(&t2);

        ASSERT_EQ(written, 9);
        ASSERT_EQ(t2.size(), written);
        ASSERT_EQ(t2.pos(), written);
    }
}
