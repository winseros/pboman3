#include "io/bs/filebasedbinarysource.h"
#include <QTemporaryFile>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace pboman3::test {
    TEST(FileBasedBinarySource, WriteRaw_Writes_When_Buffer_Size_Less_Than_Data_Size) {
        //create a binary source
        QTemporaryFile sourceFile;
        sourceFile.open();
        for (char i = 0; i < 10; i++) {
            sourceFile.write(&i, sizeof i);
        }
        sourceFile.close();

        //call the service
        QTemporaryFile targetFile;
        targetFile.open();
        FileBasedBinarySource bs(sourceFile.fileName(), 5);
        bs.writeRaw(&targetFile, []() { return false; });
        targetFile.close();

        //assert the file content
        QFile f(targetFile.fileName());
        f.open(QIODeviceBase::ReadOnly);
        const QByteArray data = f.readAll();
        f.close();

        ASSERT_THAT(data, testing::ElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8, 9));
    }

    TEST(FileBasedBinarySource, WriteRaw_Writes_When_Buffer_Size_Greater_Than_Data_Size) {
        //create a binary source
        QTemporaryFile sourceFile;
        sourceFile.open();
        for (char i = 0; i < 10; i++) {
            sourceFile.write(&i, sizeof i);
        }
        sourceFile.close();

        //call the service
        QTemporaryFile targetFile;
        targetFile.open();
        FileBasedBinarySource bs(sourceFile.fileName(), 100);
        bs.writeRaw(&targetFile, []() { return false; });
        targetFile.close();

        //assert the file content
        QFile f(targetFile.fileName());
        f.open(QIODeviceBase::ReadOnly);
        const QByteArray data = f.readAll();
        f.close();

        ASSERT_THAT(data, testing::ElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8, 9));
    }

    TEST(FileBasedBinarySource, WriteDecompressed_Writes_Raw_Data) {
        //create a binary source
        QTemporaryFile sourceFile;
        sourceFile.open();
        for (char i = 0; i < 10; i++) {
            sourceFile.write(&i, sizeof i);
        }
        sourceFile.close();

        //call the service
        QTemporaryFile targetFile;
        targetFile.open();
        FileBasedBinarySource bs(sourceFile.fileName(), 100);
        bs.writeRaw(&targetFile, []() { return false; });
        targetFile.close();

        //assert the file content
        QFile f(targetFile.fileName());
        f.open(QIODeviceBase::ReadOnly);
        const QByteArray data = f.readAll();
        f.close();

        ASSERT_THAT(data, testing::ElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8, 9));
    }

    TEST(FileBasedBinarySource, WriteCompressed_Compresses_Data) {
        //create a binary source
        QTemporaryFile sourceFile;
        sourceFile.open();
        sourceFile.write(QByteArray("a             b"));
        sourceFile.close();

        //call the service
        QTemporaryFile targetFile;
        targetFile.open();
        FileBasedBinarySource bs(sourceFile.fileName(), 100);
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
