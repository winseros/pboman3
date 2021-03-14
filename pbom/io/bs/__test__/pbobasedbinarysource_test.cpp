#include "io/bs/pbobasedbinarysource.h"
#include <QTemporaryFile>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace pboman3::test {
     TEST(PboBasedBinarySource, WriteDecompressed_Writes_When_Buffer_Size_Less_Than_Data_Size) {
        //create a binary source
        QTemporaryFile sourceFile;
        sourceFile.open();
        for (char i = 0; i < 10; i++) {
            sourceFile.write(&i, sizeof i);
        }
        sourceFile.close();

        //call the service
        const PboDataInfo dataInfo( 8, 8, 1 );
        QTemporaryFile targetFile;
        targetFile.open();
        PboBasedBinarySource bs(sourceFile.fileName(), dataInfo, 5);
        bs.writeDecompressed(&targetFile, []() { return false; });
        targetFile.close();

        //assert the file content
        QFile f(targetFile.fileName());
        f.open(QIODeviceBase::ReadOnly);
        const QByteArray data = f.readAll();
        f.close();

        ASSERT_THAT(data, testing::ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));
    }

    TEST(PboBasedBinarySource, WriteDecompressed_Writes_When_Buffer_Size_Greater_Than_Data_Size) {
        //create a binary source
        QTemporaryFile sourceFile;
        sourceFile.open();
        for (char i = 0; i < 10; i++) {
            sourceFile.write(&i, sizeof i);
        }
        sourceFile.close();

        //call the service
        const PboDataInfo dataInfo( 8, 8, 1 );
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

        ASSERT_THAT(data, testing::ElementsAre(1, 2, 3, 4, 5, 6, 7, 8));
    }
}
