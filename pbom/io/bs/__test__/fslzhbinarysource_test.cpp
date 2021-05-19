#include "io/bs/fslzhbinarysource.h"
#include <QTemporaryFile>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace pboman3::test {
    TEST(FsLzhBinarySourceTest, WriteToPbo_Compresses_Data) {
        //create a binary source
        QTemporaryFile sourceFile;
        sourceFile.open();
        sourceFile.write(QByteArray("a             b"));
        sourceFile.close();

        //call the service
        QTemporaryFile targetFile;
        targetFile.open();
        FsLzhBinarySource bs(sourceFile.fileName(), 100);
        bs.writeToPbo(&targetFile, []() { return false; });
        targetFile.close();

        //assert the file content
        QFile f(targetFile.fileName());
        f.open(QIODeviceBase::ReadOnly);
        const QByteArray data = f.readAll();
        f.close();

        ASSERT_THAT(data, testing::ElementsAre(0x05, 0x61, 0x0E, 0x0A, 0x62, 0x63, 0x02, 0x00, 0x00));
    }

    TEST(FsLzhBinarySourceTest, IsCompressed_Returns_False) {
        //create a binary source
        QTemporaryFile sourceFile;
        sourceFile.open();
        sourceFile.close();

        //call the service
        const FsLzhBinarySource bs(sourceFile.fileName());

        //assert the file
        ASSERT_TRUE(bs.isCompressed());
    }
}
