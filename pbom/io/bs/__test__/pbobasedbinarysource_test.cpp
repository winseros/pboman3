#include "io/bs/pbobasedbinarysource.h"
#include <QTemporaryFile>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace pboman3::test {
    TEST(PboBasedBinarySource, WriteRaw_Writes_When_Buffer_Size_Less_Than_Data_Size) {
        //create a binary source
        QTemporaryFile sourceFile;
        sourceFile.open();
        for (char i = 0; i < 10; i++) {
            sourceFile.write(&i, sizeof i);
        }
        sourceFile.close();

        //call the service
        const PboDataInfo dataInfo(8, 8, 1);
        QTemporaryFile targetFile;
        targetFile.open();
        PboBasedBinarySource bs(sourceFile.fileName(), dataInfo, 5);
        bs.writeRaw(&targetFile, []() { return false; });
        targetFile.close();

        //assert the file content
        QFile f(targetFile.fileName());
        f.open(QIODeviceBase::ReadOnly);
        const QByteArray data = f.readAll();
        f.close();

        ASSERT_THAT(data, testing::ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));
    }

    TEST(PboBasedBinarySource, WriteRaw_Writes_When_Buffer_Size_Greater_Than_Data_Size) {
        //create a binary source
        QTemporaryFile sourceFile;
        sourceFile.open();
        for (char i = 0; i < 10; i++) {
            sourceFile.write(&i, sizeof i);
        }
        sourceFile.close();

        //call the service
        const PboDataInfo dataInfo(8, 8, 1);
        QTemporaryFile targetFile;
        targetFile.open();
        PboBasedBinarySource bs(sourceFile.fileName(), dataInfo, 100);
        bs.writeRaw(&targetFile, []() { return false; });
        targetFile.close();

        //assert the file content
        QFile f(targetFile.fileName());
        f.open(QIODeviceBase::ReadOnly);
        const QByteArray data = f.readAll();
        f.close();

        ASSERT_THAT(data, testing::ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));
    }

    TEST(PboBasedBinarySource, WriteDecompressed_Writes_Raw_Data_If_Not_Compressed) {
        //create a binary source
        QTemporaryFile sourceFile;
        sourceFile.open();
        sourceFile.write(QByteArray("decompressed data")); //17 chars
        sourceFile.close();

        //call the service
        const PboDataInfo dataInfo(17, 17, 0); //the file is marked as compressed
        QTemporaryFile targetFile;
        targetFile.open();
        PboBasedBinarySource bs(sourceFile.fileName(), dataInfo, 100);
        bs.writeDecompressed(&targetFile, []() { return false; });
        targetFile.close();

        //assert the file content
        QFile f(targetFile.fileName());
        f.open(QIODeviceBase::ReadOnly);
        const QByteArray data = f.readAll();
        f.close();

        ASSERT_EQ(data, QString("decompressed data"));
    }

    TEST(PboBasedBinarySource, WriteDecompressed_Decompresses_If_Data_Compressed) {
        //create a binary source
        QTemporaryFile sourceFile;
        sourceFile.open();
        sourceFile.write(QByteArray("\x05\x61\x0E\x0A\x62\x63\x02\x00\x00", 9)); //9 chars
        sourceFile.close();

        //call the service
        const PboDataInfo dataInfo(15, 9, 0); //the file is marked as compressed
        QTemporaryFile targetFile;
        targetFile.open();
        PboBasedBinarySource bs(sourceFile.fileName(), dataInfo, 100);
        bs.writeDecompressed(&targetFile, []() { return false; });
        targetFile.close();

        //assert the file content
        QFile f(targetFile.fileName());
        f.open(QIODeviceBase::ReadOnly);
        const QByteArray data = f.readAll();
        f.close();

        ASSERT_EQ(data, QString("a             b"));
    }

    TEST(PboBasedBinarySource, WriteDecompressed_Writes_Raw_If_Could_Not_Decompres) {
        //create a binary source
        QTemporaryFile sourceFile;
        sourceFile.open();
        sourceFile.write(QByteArray("corrupted decompress data")); //25 chars
        sourceFile.close();

        //call the service
        const PboDataInfo dataInfo(30, 25, 0); //the file is marked as compressed
        QTemporaryFile targetFile;
        targetFile.open();
        PboBasedBinarySource bs(sourceFile.fileName(), dataInfo, 100);
        bs.writeDecompressed(&targetFile, []() { return false; });
        targetFile.close();

        //assert the file content
        QFile f(targetFile.fileName());
        f.open(QIODeviceBase::ReadOnly);
        const QByteArray data = f.readAll();
        f.close();

        ASSERT_EQ(data, QString("corrupted decompress data"));
    }

    TEST(PboBasedBinarySource, WriteCompressed_Writes_Raw_If_Already_Compressed) {
        //create a binary source
        QTemporaryFile sourceFile;
        sourceFile.open();
        sourceFile.write(QByteArray("\x05\x61\x0E\x0A\x62\x63\x02\x00\x00", 9)); //9 chars
        sourceFile.close();

        //call the service
        const PboDataInfo dataInfo(15, 9, 0); //the file is marked as compressed
        QTemporaryFile targetFile;
        targetFile.open();
        PboBasedBinarySource bs(sourceFile.fileName(), dataInfo, 100);
        bs.writeCompressed(&targetFile, []() { return false; });
        targetFile.close();

        //assert the file content
        QFile f(targetFile.fileName());
        f.open(QIODeviceBase::ReadOnly);
        const QByteArray data = f.readAll();
        f.close();

        ASSERT_THAT(data, testing::ElementsAre(0x05, 0x61, 0x0E, 0x0A, 0x62, 0x63, 0x02, 0x00, 0x00));
    }

    TEST(PboBasedBinarySource, WriteCompressed_Compresses_Data) {
        //create a binary source
        QTemporaryFile sourceFile;
        sourceFile.open();
        sourceFile.write(QByteArray("a             b"));
        sourceFile.close();

        //call the service
        const PboDataInfo dataInfo(15, 15, 0);
        QTemporaryFile targetFile;
        targetFile.open();
        PboBasedBinarySource bs(sourceFile.fileName(), dataInfo, 100);
        bs.writeCompressed(&targetFile, []() { return false; });
        targetFile.close();

        //assert the file content
        QFile f(targetFile.fileName());
        f.open(QIODeviceBase::ReadOnly);
        const QByteArray data = f.readAll();
        f.close();

        ASSERT_THAT(data, testing::ElementsAre(0x05, 0x61, 0x0E, 0x0A, 0x62, 0x63, 0x02, 0x00, 0x00));
    }
}
