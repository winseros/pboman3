#include "io/lzh/compressionbuffer.h"
#include <QByteArray>
#include <QTemporaryFile>
#include <gtest/gtest.h>

namespace pboman3::test {
    TEST(CompressionBufferTest, Add1_Adds_Buffer_Longer_Than_Space_Remaining) {
        QTemporaryFile t1;
        t1.open();
        t1.write(QByteArray("abcdef"));//6 chars
        t1.seek(0);

        QTemporaryFile t2;
        t2.open();
        t2.write(QByteArray("ghijk"));//5 chars
        t2.seek(0);

        CompressionBuffer buf(10);
        buf.add(&t1, t1.size());
        buf.add(&t2, t2.size());

        const QByteArray expected("bcdefghijk");//10 chars
        const BufferIntersection intersection = buf.intersect(expected, expected.length());

        ASSERT_EQ(intersection.position, 0);
        ASSERT_EQ(intersection.length, expected.length());
        ASSERT_EQ(buf.getFulfillment(), 10);
    }

    TEST(CompressionBufferTest, Add1_Adds_Buffer_Not_Longer_Than_Space_Remaining) {
        QTemporaryFile t1;
        t1.open();
        t1.write(QByteArray("abcdef"));//6 chars
        t1.seek(0);

        QTemporaryFile t2;
        t2.open();
        t2.write(QByteArray("ghij"));//4 chars
        t2.seek(0);

        CompressionBuffer buf(10);
        buf.add(&t1, t1.size());
        buf.add(&t2, t2.size());

        const QByteArray expected("abcdefghij");//10 chars
        const BufferIntersection intersection = buf.intersect(expected, expected.length());

        ASSERT_EQ(intersection.position, 0);
        ASSERT_EQ(intersection.length, expected.length());
        ASSERT_EQ(buf.getFulfillment(), 10);
    }

    TEST(CompressionBufferTest, Add2_Adds_Buffer_Longer_Than_Space_Remaining) {
        QTemporaryFile t1;
        t1.open();
        t1.write(QByteArray("abcdefghij"));//10 chars
        t1.seek(0);

        CompressionBuffer buf(10);
        buf.add(&t1, t1.size());
        buf.add('k');

        const QByteArray expected("bcdefghijk");//10 chars
        const BufferIntersection intersection = buf.intersect(expected, expected.length());

        ASSERT_EQ(intersection.position, 0);
        ASSERT_EQ(intersection.length, expected.length());
        ASSERT_EQ(buf.getFulfillment(), 10);
    }

    TEST(CompressionBufferTest, Add2_Adds_Buffer_Not_Longer_Than_Space_Remaining) {
        QTemporaryFile t1;
        t1.open();
        t1.write(QByteArray("abcdefghi"));//9 chars
        t1.seek(0);

        CompressionBuffer buf(10);
        buf.add(&t1, t1.size());
        buf.add('j');

        const QByteArray expected("abcdefghij");//10 chars
        const BufferIntersection intersection = buf.intersect(expected, expected.length());

        ASSERT_EQ(intersection.position, 0);
        ASSERT_EQ(intersection.length, expected.length());
        ASSERT_EQ(buf.getFulfillment(), 10);
    }

    TEST(CompressionBufferTest, Intersect_Returns_Negative_If_Input_Buffer_Is_Empty) {
        CompressionBuffer buffer;
        const BufferIntersection intersection = buffer.intersect(QByteArray(), 0);

        ASSERT_EQ(intersection.position, -1);
        ASSERT_EQ(intersection.length, 0);
    }

    TEST(CompressionBufferTest, Intersect_Returns_Negative_If_Stack_Is_Empty) {
        CompressionBuffer buffer;

        QByteArray arr;
        arr.resize(10);
        const BufferIntersection intersection = buffer.intersect(arr, arr.size());

        ASSERT_EQ(intersection.position, -1);
        ASSERT_EQ(intersection.length, 0);
    }

    TEST(CompressionBufferTest, Intersect_Returns_Negative_If_There_Were_No_Intersections) {
        QTemporaryFile t1;
        t1.open();
        t1.write(QByteArray("aabbbbcccc"));
        t1.seek(0);

        CompressionBuffer buffer;
        buffer.add(&t1, t1.size());

        const QByteArray arr("ddd");
        const BufferIntersection intersection = buffer.intersect(arr, arr.size());

        ASSERT_EQ(intersection.position, -1);
        ASSERT_EQ(intersection.length, 0);
    }

    TEST(CompressionBufferTest, Intersect_Returns_The_Most_Suitable_Intersection) {
        QTemporaryFile t1;
        t1.open();
        t1.write(QByteArray("aabbbbcccc"));
        t1.seek(0);

        CompressionBuffer buffer;
        buffer.add(&t1, t1.size());

        const QByteArray arr("bbccc");
        const BufferIntersection intersection = buffer.intersect(arr, arr.size());

        ASSERT_EQ(intersection.position, 4);
        ASSERT_EQ(intersection.length, 5);
    }

    TEST(CompressionBufferTest, Intersect_Returns_A_Partial_Intersection_At_The_End_Of_The_Data) {
        QTemporaryFile t1;
        t1.open();
        t1.write(QByteArray("aabbbbcccd"));
        t1.seek(0);

        CompressionBuffer buffer;
        buffer.add(&t1, t1.size());

        const QByteArray arr("ccdea");
        const BufferIntersection intersection = buffer.intersect(arr, arr.size());

        ASSERT_EQ(intersection.position, 7);
        ASSERT_EQ(intersection.length, 3);
    }

    TEST(CompressionBufferTest, Intersect_Returns_A_Partial_Intersection_In_The_Middle_Of_The_Data) {
        QTemporaryFile t1;
        t1.open();
        t1.write(QByteArray("aaaabbbcdd"));
        t1.seek(0);

        CompressionBuffer buffer;
        buffer.add(&t1, t1.size());

        const QByteArray arr("bbcea");
        const BufferIntersection intersection = buffer.intersect(arr, arr.size());

        ASSERT_EQ(intersection.position, 5);
        ASSERT_EQ(intersection.length, 3);
    }

    TEST(CompressionBufferTest, CheckWhitespace_Returns_Zero_If_Buffer_Starts_From_Non_Whitespace) {
        CompressionBuffer buf;

        QByteArray arr("aa");

        const qint64 whitespace = buf.checkWhitespace(arr, arr.length());

        ASSERT_EQ(whitespace, 0);
    }

    TEST(CompressionBufferTest, CheckWhitespace_Returns_Number_Of_Sequential_Whitespaces) {
        CompressionBuffer buf;

        QByteArray arr("     ");

        const qint64 whitespace = buf.checkWhitespace(arr, arr.length());

        ASSERT_EQ(whitespace, 5);
    }

    TEST(CompressionBufferTest, CheckWhitespace_Returns_Number_Of_Sequential_Whitespaces_Ending_With_Non_Whitespace) {
        CompressionBuffer buf;

        QByteArray arr("     a");

        const qint64 whitespace = buf.checkWhitespace(arr, arr.length());

        ASSERT_EQ(whitespace, 5);
    }

    TEST(CompressionBufferTest, CheckSequence_Returns_Valid_Result_If_There_Is_No_Match_At_All) {
        QTemporaryFile t1;
        t1.open();
        t1.write(QByteArray("abcd"));
        t1.seek(0);

        CompressionBuffer buffer;
        buffer.add(&t1, t1.size());

        const QByteArray arr("efg");
        const SequenceInspection inspection = buffer.checkSequence(arr, arr.size());

        ASSERT_EQ(inspection.sequenceBytes, 0);
        ASSERT_EQ(inspection.sourceBytes, 0);
    }

    TEST(CompressionBufferTest, CheckSequence_Returns_Valid_Result_If_There_A_Regular_Intersection) {
        QTemporaryFile t1;
        t1.open();
        t1.write(QByteArray("abcdef"));
        t1.seek(0);

        CompressionBuffer buffer;
        buffer.add(&t1, t1.size());

        const QByteArray arr("efg");
        const SequenceInspection inspection = buffer.checkSequence(arr, arr.size());

        ASSERT_EQ(inspection.sequenceBytes, 2);
        ASSERT_EQ(inspection.sourceBytes, 2);
    }

    TEST(CompressionBufferTest, CheckSequence_Returns_Valid_Result_If_There_A_Sequence_Intersection) {
        QTemporaryFile t1;
        t1.open();
        t1.write(QByteArray("abcd"));
        t1.seek(0);

        CompressionBuffer buffer;
        buffer.add(&t1, t1.size());

        const QByteArray arr("cdcdcdc");
        const SequenceInspection inspection = buffer.checkSequence(arr, arr.size());

        ASSERT_EQ(inspection.sequenceBytes, 2);
        ASSERT_EQ(inspection.sourceBytes, 7);
    }
}
