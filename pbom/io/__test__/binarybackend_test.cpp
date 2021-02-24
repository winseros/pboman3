#include <QDir>
#include <QTemporaryFile>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "io/binarybackend.h"

namespace pboman3::test {
    /*class MockBinaryBackend : public BinaryBackend {
    public:
        MockBinaryBackend()
            : BinaryBackend(10) {
        }

        const QMap<QString, QString>& extracted() const {
            return BinaryBackend::extracted();
        }

        const QString& temp() const {
            return BinaryBackend::temp();
        }
    };

    TEST(BinaryBackend, Initialize_Creates_Folder_In_Temp) {
        MockBinaryBackend be;
        be.initialize();

        ASSERT_TRUE(QDir::temp().exists(be.temp()));
    }

    TEST(BinaryBackend, ExtractSingle_Extracts_File_And_Puts_It_To_Disk) {
        //create a binary source
        QTemporaryFile t;
        t.open();
        for (char i = 0; i < 20; i++) {
            t.write(&i, sizeof i);
        }
        t.close();

        //pbo entry
        PboEntry_ e1("some/file/path.txt", PboPackingMethod::Uncompressed, 0, 0,
                     0, 5);
        e1.dataOffset = 10;

        //call the service
        MockBinaryBackend be;
        be.initialize();

        const QString path = be.extractSingle(&e1, t.fileName()).takeResult();

        //expected temp folder location
        QDir tempDir(QDir::temp());
        tempDir.cd(be.temp());

        //assert the result
        ASSERT_TRUE(QFile::exists(path));
        ASSERT_TRUE(path.startsWith(tempDir.absolutePath() + QDir::separator()));
        ASSERT_TRUE(be.extracted().contains(e1.fileName));
        ASSERT_EQ(be.extracted()[e1.fileName], path);
    }

    TEST(BinaryBackend, ExtractSingle_Extracts_File_Sets_Its_Metadata) {
        //create a binary source
        QTemporaryFile t;
        t.open();
        for (char i = 0; i < 20; i++) {
            t.write(&i, sizeof i);
        }
        t.close();

        //mock last-modified timestamp
        QDateTime changeDate(QDateTime::currentDateTime());
        changeDate = changeDate.addDays(-1);
        const qint64 timestamp = changeDate.toSecsSinceEpoch();

        //pbo entry
        PboEntry_ e1("some/file/path.txt", PboPackingMethod::Uncompressed, 0, 0,
                     static_cast<int>(timestamp), 5);
        e1.dataOffset = 10;

        //call the service
        MockBinaryBackend be;
        be.initialize();

        const QString path = be.extractSingle(&e1, t.fileName()).takeResult();

        //assert file metadata
        const QFileInfo fi(path);
        qDebug() << fi.lastModified();
        qDebug() << changeDate;
        ASSERT_EQ(fi.lastModified(), QDateTime::fromSecsSinceEpoch(timestamp));
    }

    TEST(BinaryBackend, ExtractSingle_Extracts_File_With_Correct_Content_If_Chunk_Less_Then_Buffer_Size) {
        //create a binary source
        QTemporaryFile t;
        t.open();
        for (char i = 0; i < 20; i++) {
            t.write(&i, sizeof i);
        }
        t.close();

        //pbo entry
        PboEntry_ e1("some/file/path.txt", PboPackingMethod::Uncompressed, 0, 0,
                     0, 5);
        e1.dataOffset = 10;

        //call the service
        MockBinaryBackend be;
        be.initialize();
        
        const QString path = be.extractSingle(&e1, t.fileName()).takeResult();

        //assert the file content
        QFile f(path);
        f.open(QIODeviceBase::ReadOnly);
        const QByteArray data = f.readAll();
        f.close();

        ASSERT_EQ(data.size(), e1.dataSize);
        ASSERT_THAT(data, testing::ElementsAre(10, 11, 12, 13, 14));
    }

    TEST(BinaryBackend, ExtractSingle_Extracts_File_With_Correct_Content_If_Chunk_Greater_Then_Buffer_Size) {
        //create a binary source
        QTemporaryFile t;
        t.open();
        for (char i = 0; i < 20; i++) {
            t.write(&i, sizeof i);
        }
        t.close();

        //pbo entry
        PboEntry_ e1("some/file/path.txt", PboPackingMethod::Uncompressed, 0, 0,
                     0, 11);
        e1.dataOffset = 3;

        //call the service
        MockBinaryBackend be;
        be.initialize();

        const QString path = be.extractSingle(&e1, t.fileName()).takeResult();

        //assert the file content
        QFile f(path);
        f.open(QIODeviceBase::ReadOnly);
        const QByteArray data = f.readAll();
        f.close();

        ASSERT_EQ(data.size(), e1.dataSize);
        ASSERT_THAT(data, testing::ElementsAre(3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13));
    }*/
}
