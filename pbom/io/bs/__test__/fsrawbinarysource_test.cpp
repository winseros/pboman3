#include "io/bs/fsrawbinarysource.h"
#include <QFileInfo>
#include <QTemporaryFile>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace pboman3::io::test {
    TEST(FsRawBinarySource, WriteToPbo_Writes_When_Buffer_Size_Less_Than_Data_Size) {
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
        FsRawBinarySource bs(sourceFile.fileName(), 5);
        bs.open();
        bs.writeToPbo(&targetFile, []() { return false; });
        targetFile.close();

        //assert the file content
        QFile f(targetFile.fileName());
        f.open(QIODeviceBase::ReadOnly);
        const QByteArray data = f.readAll();
        f.close();

        ASSERT_THAT(data, testing::ElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8, 9));
    }

    TEST(FsRawBinarySource, WriteToFs_Writes_When_Buffer_Size_Greater_Than_Data_Size) {
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
        FsRawBinarySource bs(sourceFile.fileName(), 100);
        bs.open();
        bs.writeToFs(&targetFile, []() { return false; });
        targetFile.close();

        //assert the file content
        QFile f(targetFile.fileName());
        f.open(QIODeviceBase::ReadOnly);
        const QByteArray data = f.readAll();
        f.close();

        ASSERT_THAT(data, testing::ElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8, 9));
    }

    TEST(FsRawBinarySource, ReadOriginalSize_Returns_File_Size) {
        //create a binary source
        QTemporaryFile sourceFile;
        sourceFile.open();
        for (char i = 0; i < 10; i++) {
            sourceFile.write(&i, sizeof i);
        }
        sourceFile.close();

        //call the service
        const FsRawBinarySource bs(sourceFile.fileName(), 100);

        //assert the file
        ASSERT_EQ(bs.readOriginalSize(), 10);
    }

    TEST(FsRawBinarySource, ReadTimestamp_Returns_Timestamp) {
        //create a binary source
        QTemporaryFile sourceFile;
        sourceFile.open();
        sourceFile.close();

        //call the service
        const FsRawBinarySource bs(sourceFile.fileName(), 100);

        //assert the file
        const QFileInfo fi(sourceFile.fileName());
        ASSERT_EQ(bs.readTimestamp(), fi.lastModified().toSecsSinceEpoch());
    }

    TEST(FsRawBinarySource, IsCompressed_Returns_False) {
        //create a binary source
        QTemporaryFile sourceFile;
        sourceFile.open();
        sourceFile.close();

        //call the service
        const FsRawBinarySource bs(sourceFile.fileName());

        //assert the file
        ASSERT_FALSE(bs.isCompressed());
    }
}
