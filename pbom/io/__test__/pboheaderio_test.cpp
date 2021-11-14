#include "io/pboheaderio.h"
#include <QByteArray>
#include <QSharedPointer>
#include <QTemporaryFile>
#include <gtest/gtest.h>

namespace pboman3::io::test {
    // ReSharper disable once CppInconsistentNaming
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

        const QSharedPointer<PboEntry> e = io.readNextEntry();
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

        const QSharedPointer<PboEntry> e = io.readNextEntry();
        ASSERT_TRUE(e);
        ASSERT_TRUE(e->isBoundary());
    }

    // ReSharper disable once CppInconsistentNaming
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

        const QSharedPointer<PboHeader> e = io.readNextHeader();
        ASSERT_FALSE(e);
    }

    INSTANTIATE_TEST_SUITE_P(ReadNextHeader, PboHeaderIOTest_ReadNextHeader, testing::Values(0));

    TEST(PboHeaderIOTest, WriteEntry_Writes) {
        const PboEntry e1("some-file1", PboPackingMethod::Packed, 0x01010101, 0x02020202, 0x03030303, 0x04040404);
        const PboEntry e2("some-file2", PboPackingMethod::Uncompressed, 0x05050505, 0x06060606, 0x07070707, 0x08080808);
        QTemporaryFile t;
        ASSERT_TRUE(t.open());

        PboFile f{t.fileName()};
        f.open(QIODeviceBase::WriteOnly);
        const PboHeaderIO io(&f);
        io.writeEntry(e1);
        io.writeEntry(e2);
        f.close();
        
        const QByteArray all = t.readAll();

        const PboPackingMethod pmE1 = e1.packingMethod();
        const qint32 osE1 = e1.originalSize();
        const qint32 rsE1 = e1.reserved();
        const qint32 tsE1 = e1.timestamp();
        const qint32 dsE1 = e1.dataSize();

        QByteArray expected;
        expected.append(e1.fileName().toUtf8()).append(1, 0);
        expected.append(reinterpret_cast<const char*>(&pmE1), sizeof pmE1);
        expected.append(reinterpret_cast<const char*>(&osE1), sizeof osE1);
        expected.append(reinterpret_cast<const char*>(&rsE1), sizeof rsE1);
        expected.append(reinterpret_cast<const char*>(&tsE1), sizeof tsE1);
        expected.append(reinterpret_cast<const char*>(&dsE1), sizeof dsE1);

        const PboPackingMethod pmE2 = e2.packingMethod();
        const qint32 osE2 = e2.originalSize();
        const qint32 rsE2 = e2.reserved();
        const qint32 tsE2 = e2.timestamp();
        const qint32 dsE2 = e2.dataSize();

        expected.append(e2.fileName().toUtf8()).append(1, 0);
        expected.append(reinterpret_cast<const char*>(&pmE2), sizeof pmE2);
        expected.append(reinterpret_cast<const char*>(&osE2), sizeof osE2);
        expected.append(reinterpret_cast<const char*>(&rsE2), sizeof rsE2);
        expected.append(reinterpret_cast<const char*>(&tsE2), sizeof tsE2);
        expected.append(reinterpret_cast<const char*>(&dsE2), sizeof dsE2);

        ASSERT_EQ(expected.compare(all), 0);
    }

    TEST(PboHeaderIOTest, WriteHeader_Writes) {
        const PboHeader h1("h1", "v1");
        const PboHeader h2("h2", "v2");
        const PboHeader h3 = PboHeader::makeBoundary();
        QTemporaryFile t;
        ASSERT_TRUE(t.open());

        PboFile f{t.fileName()};
        f.open(QIODeviceBase::WriteOnly);
        const PboHeaderIO io(&f);
        io.writeHeader(h1);
        io.writeHeader(h2);
        io.writeHeader(h3);
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
