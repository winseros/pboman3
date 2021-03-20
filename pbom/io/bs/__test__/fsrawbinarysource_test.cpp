#include "io/bs/fsrawbinarysource.h"
#include <QTemporaryFile>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace pboman3::test {
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
        bs.writeToFs(&targetFile, []() { return false; });
        targetFile.close();

        //assert the file content
        QFile f(targetFile.fileName());
        f.open(QIODeviceBase::ReadOnly);
        const QByteArray data = f.readAll();
        f.close();

        ASSERT_THAT(data, testing::ElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8, 9));
    }
}
