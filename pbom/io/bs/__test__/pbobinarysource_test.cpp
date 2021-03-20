#include "io/bs/pbobinarysource.h"
#include <QTemporaryFile>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace pboman3::test {
    TEST(PboBinarySource, WriteToPbo_Writes_When_Buffer_Size_Less_Than_Data_Size) {
        //create a binary source
        QTemporaryFile sourceFile;
        sourceFile.open();
        for (char i = 0; i < 10; i++) {
            sourceFile.write(&i, sizeof i);
        }
        sourceFile.close();

        //call the service
        const PboDataInfo dataInfo(8, 8, 1);//not compressed
        QTemporaryFile targetFile;
        targetFile.open();
        PboBinarySource bs(sourceFile.fileName(), dataInfo, 5);
        bs.writeToPbo(&targetFile, []() { return false; });
        targetFile.close();

        //assert the file content
        QFile f(targetFile.fileName());
        f.open(QIODeviceBase::ReadOnly);
        const QByteArray data = f.readAll();
        f.close();

        ASSERT_THAT(data, testing::ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));
    }

    TEST(PboBinarySource, WriteToPbo_Writes_When_Buffer_Size_Greater_Than_Data_Size) {
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
        PboBinarySource bs(sourceFile.fileName(), dataInfo, 100);
        bs.writeToPbo(&targetFile, []() { return false; });
        targetFile.close();

        //assert the file content
        QFile f(targetFile.fileName());
        f.open(QIODeviceBase::ReadOnly);
        const QByteArray data = f.readAll();
        f.close();

        ASSERT_THAT(data, testing::ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));
    }

    TEST(PboBinarySource, WriteToFs_Writes_Raw_Data_If_Not_Compressed) {
        //create a binary source
        QTemporaryFile sourceFile;
        sourceFile.open();
        sourceFile.write(QByteArray("decompressed data")); //17 chars
        sourceFile.close();

        //call the service
        const PboDataInfo dataInfo(17, 17, 0); //the file is marked as compressed
        QTemporaryFile targetFile;
        targetFile.open();
        PboBinarySource bs(sourceFile.fileName(), dataInfo, 100);
        bs.writeToFs(&targetFile, []() { return false; });
        targetFile.close();

        //assert the file content
        QFile f(targetFile.fileName());
        f.open(QIODeviceBase::ReadOnly);
        const QByteArray data = f.readAll();
        f.close();

        ASSERT_EQ(data, QString("decompressed data"));
    }

    TEST(PboBinarySource, WriteToFs_Decompresses_If_Data_Compressed) {
        //create a binary source
        QTemporaryFile sourceFile;
        sourceFile.open();
        sourceFile.write(QByteArray("\x05\x61\x0E\x0A\x62\x63\x02\x00\x00", 9)); //9 chars
        sourceFile.close();

        //call the service
        const PboDataInfo dataInfo(15, 9, 0); //the file is marked as compressed
        QTemporaryFile targetFile;
        targetFile.open();
        PboBinarySource bs(sourceFile.fileName(), dataInfo, 100);
        bs.writeToFs(&targetFile, []() { return false; });
        targetFile.close();

        //assert the file content
        QFile f(targetFile.fileName());
        f.open(QIODeviceBase::ReadOnly);
        const QByteArray data = f.readAll();
        f.close();

        ASSERT_EQ(data, QString("a             b"));
    }

    TEST(PboBinarySource, WriteToFs_Writes_Raw_If_Could_Not_Decompres) {
        //create a binary source
        QTemporaryFile sourceFile;
        sourceFile.open();
        sourceFile.write(QByteArray("corrupted decompress data")); //25 chars
        sourceFile.close();

        //call the service
        const PboDataInfo dataInfo(30, 25, 0); //the file is marked as compressed
        QTemporaryFile targetFile;
        targetFile.open();
        PboBinarySource bs(sourceFile.fileName(), dataInfo, 100);
        bs.writeToFs(&targetFile, []() { return false; });
        targetFile.close();

        //assert the file content
        QFile f(targetFile.fileName());
        f.open(QIODeviceBase::ReadOnly);
        const QByteArray data = f.readAll();
        f.close();

        ASSERT_EQ(data, QString("corrupted decompress data"));
    }
}
