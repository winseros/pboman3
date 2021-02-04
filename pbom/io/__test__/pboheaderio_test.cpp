#include "io/pboheaderio.h"
#include <QByteArray>
#include <QTemporaryFile>
#include <gtest/gtest.h>

namespace pboman3::test {
    class PboHeaderIOTest_ReadNextEntry : public testing::TestWithParam<int> {
    };

    TEST_P(PboHeaderIOTest_ReadNextEntry, Returns_Null) {
        const int bytesCount = GetParam();

        QByteArray arr;
        arr.append(bytesCount, 0);

        QTemporaryFile t;
        ASSERT_TRUE(t.open());
        t.write(arr);
        t.close();

        PboFile f(t.fileName());
        f.open(QIODeviceBase::ReadOnly);

        const PboHeaderIO io(&f);

        const unique_ptr<PboEntry_> e = io.readNextEntry();
        ASSERT_FALSE(e);
    }

    INSTANTIATE_TEST_SUITE_P(ReadNextEntry, PboHeaderIOTest_ReadNextEntry, testing::Values(0, 10, 20));

    TEST(PboHeaderIOTest, ReadNextEntry_Returns_Boundary) {
        QByteArray arr;
        arr.append(21, 0);

        QTemporaryFile t;
        ASSERT_TRUE(t.open());
        t.write(arr);
        t.close();

        PboFile f(t.fileName());
        f.open(QIODeviceBase::ReadOnly);

        const PboHeaderIO io(&f);

        const unique_ptr<PboEntry_> e = io.readNextEntry();
        ASSERT_TRUE(e);
        ASSERT_TRUE(e->isBoundary());
    }

    class PboHeaderIOTest_ReadNextHeader : public testing::TestWithParam<int> {
    };

    TEST_P(PboHeaderIOTest_ReadNextHeader, Returns_Null) {
        const int bytesCount = GetParam();

        QByteArray arr;
        arr.append(bytesCount, 0);

        QTemporaryFile t;
        ASSERT_TRUE(t.open());
        t.write(arr);
        t.close();

        PboFile f(t.fileName());
        f.open(QIODeviceBase::ReadOnly);

        const PboHeaderIO io(&f);

        const unique_ptr<PboHeader> e = io.readNextHeader();
        ASSERT_FALSE(e);
    }

    INSTANTIATE_TEST_SUITE_P(ReadNextHeader, PboHeaderIOTest_ReadNextHeader, testing::Values(0));

    TEST(PboHeaderIOTest, WriteEntry_Writes) {
        PboEntry e1("some-file1", PboPackingMethod::Packed, 0x01010101, 0x02020202, 0x03030303, 0x04040404);
        PboEntry e2("some-file2", PboPackingMethod::Uncompressed, 0x05050505, 0x06060606, 0x07070707, 0x08080808);
        QTemporaryFile t;
        ASSERT_TRUE(t.open());

        PboFile f{t.fileName()};
        f.open(QIODeviceBase::WriteOnly);
        const PboHeaderIO io(&f);
        io.writeEntry(&e1);
        io.writeEntry(&e2);
        f.close();
        
        const QByteArray all = t.readAll();

        QByteArray expected;
        expected.append(e1.fileName.toUtf8()).append(1, 0);
        expected.append(reinterpret_cast<const char*>(&e1.packingMethod), sizeof e1.packingMethod);
        expected.append(reinterpret_cast<const char*>(&e1.originalSize), sizeof e1.originalSize);
        expected.append(reinterpret_cast<const char*>(&e1.reserved), sizeof e1.reserved);
        expected.append(reinterpret_cast<const char*>(&e1.timestamp), sizeof e1.timestamp);
        expected.append(reinterpret_cast<const char*>(&e1.dataSize), sizeof e1.dataSize);
        expected.append(e2.fileName.toUtf8()).append(1, 0);
        expected.append(reinterpret_cast<const char*>(&e2.packingMethod), sizeof e2.packingMethod);
        expected.append(reinterpret_cast<const char*>(&e2.originalSize), sizeof e2.originalSize);
        expected.append(reinterpret_cast<const char*>(&e2.reserved), sizeof e2.reserved);
        expected.append(reinterpret_cast<const char*>(&e2.timestamp), sizeof e2.timestamp);
        expected.append(reinterpret_cast<const char*>(&e2.dataSize), sizeof e2.dataSize);

        ASSERT_EQ(expected.compare(all), 0);
    }

    TEST(PboHeaderIOTest, WriteHeader_Writes) {
        PboHeader h1("h1", "v1");
        PboHeader h2("h2", "v2");
        PboHeader h3 = PboHeader::makeBoundary();
        QTemporaryFile t;
        ASSERT_TRUE(t.open());

        PboFile f{t.fileName()};
        f.open(QIODeviceBase::WriteOnly);
        const PboHeaderIO io(&f);
        io.writeHeader(&h1);
        io.writeHeader(&h2);
        io.writeHeader(&h3);
        f.close();
        
        const QByteArray all = t.readAll();

        QByteArray expected;
        expected.append(h1.name.toUtf8()).append(1, 0);
        expected.append(h1.value.toUtf8()).append(1, 0);
        expected.append(h2.name.toUtf8()).append(1, 0);
        expected.append(h2.value.toUtf8()).append(1, 0);
        expected.append(1, 0);

        ASSERT_EQ(expected.compare(all), 0);
    }
}
