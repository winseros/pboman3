#include "io/documentwriter.h"
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <gtest/gtest.h>
#include "domain/pbodocument.h"
#include "domain/documentheaderstransaction.h"
#include "io/pboheaderreader.h"
#include "io/bs/fsrawbinarysource.h"

namespace pboman3::io::test {
    using namespace domain;
    TEST(DocumentWriterTest, Write_Writes_File_WithHeaders) {
        //mock files contents
        const QByteArray mockContent1(15, 1);
        QTemporaryFile e1;
        e1.open();
        e1.write(mockContent1);
        e1.close();

        const QByteArray mockContent2(10, 2);
        QTemporaryFile e2;
        e2.open();
        e2.write(mockContent2);
        e2.close();

        //pbo document
        PboDocument document("file.pbo");

        //pbo headers
        QSharedPointer<DocumentHeadersTransaction> tran = document.headers()->beginTransaction();
        tran->add("h1", "v1");
        tran->add("h2", "v2");
        tran->commit();
        tran.clear();

        //pbo entries with content
        PboNode* n1 = document.root()->createHierarchy(PboPath("e1.txt"));
        n1->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(e1.fileName()));
        n1->binarySource->open();
        PboNode* n2 = document.root()->createHierarchy(PboPath("f2/e2.txt"));
        n2->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(e2.fileName()));
        n2->binarySource->open();

        //write the file
        const QTemporaryDir temp;
        const QString filePath = temp.filePath("file.pbo");

        DocumentWriter writer(filePath);
        writer.write(&document, []() { return false; });

        //assert the result
        PboFile pbo(filePath);
        pbo.open(QIODeviceBase::ReadOnly);
        const PboFileHeader header = PboHeaderReader::readFileHeader(&pbo);

        //pbo header
        ASSERT_EQ(header.headers.count(), 2);
        ASSERT_EQ(header.headers.at(0)->name, "h1");
        ASSERT_EQ(header.headers.at(0)->value, "v1");
        ASSERT_EQ(header.headers.at(1)->name, "h2");
        ASSERT_EQ(header.headers.at(1)->value, "v2");

        ASSERT_EQ(header.entries.count(), 2);
        ASSERT_EQ(header.entries.at(0)->fileName(), "f2/e2.txt");
        ASSERT_EQ(header.entries.at(1)->fileName(), "e1.txt");

        //pbo contents
        pbo.seek(header.dataBlockStart);

        QByteArray contents;
        contents.resize(mockContent2.size());
        pbo.read(contents.data(), contents.size());
        ASSERT_EQ(contents, mockContent2);

        contents.resize(mockContent1.size());
        pbo.read(contents.data(), contents.size());
        ASSERT_EQ(contents, mockContent1);

        //zero byte
        QByteArray zero;
        zero.resize(1);
        pbo.read(zero.data(), zero.size());
        ASSERT_EQ(zero.at(0), 0);

        //signature
        ASSERT_EQ(document.signature().size(), 20);//sha1 is 20 bytes
        contents.resize(document.signature().size());
        pbo.read(contents.data(), contents.size());
        ASSERT_EQ(contents.size(), document.signature().size());

        //file has ended
        ASSERT_TRUE(pbo.atEnd());
    }

    class DocumentWriterTest : public testing::TestWithParam<int> {};

    TEST_P(DocumentWriterTest, Write_Cleans_Files_On_Cancel) {
        //mock files contents
        const QByteArray mockContent1(15, 1);
        QTemporaryFile e1;
        e1.open();
        e1.write(mockContent1);
        e1.close();

        //pbo document with content
        PboDocument document("file.pbo");
        PboNode* n1 = document.root()->createHierarchy(PboPath("e1.txt"));
        n1->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(e1.fileName()));
        n1->binarySource->open();

        //pbo file
        const QTemporaryDir temp;
        const QString filePath = temp.filePath("file.pbo");
        
        //call the method
        int count = 0;
        int expectedHitCount = GetParam();//experimental way
        DocumentWriter writer(filePath);
        writer.write(&document, [&count, expectedHitCount]() { count++; return count > expectedHitCount - 1; });

        ASSERT_FALSE(QFileInfo(filePath).exists());
        ASSERT_FALSE(QFileInfo(filePath + ".b").exists());
    }

    INSTANTIATE_TEST_SUITE_P(Write_Cleans_On_Cancel, DocumentWriterTest, testing::Range(1, 13));

    TEST(DocumentWriterTest, Write_Cleans_Temporary_Files_On_Write) {
        //mock files contents
        const QByteArray mockContent1(15, 1);
        QTemporaryFile e1;
        e1.open();
        e1.write(mockContent1);
        e1.close();

        //pbo file
        const QTemporaryDir temp;
        const QString filePath = temp.filePath("file.pbo");

        //pbo content structure
        PboDocument document("file.pbo");
        PboNode* n1 = document.root()->createHierarchy(PboPath("e1.txt"));
        n1->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(e1.fileName()));
        n1->binarySource->open();

        //call the method
        DocumentWriter writer(filePath);
        writer.write(&document, []() { return false; });

        //ensure no junk left
        ASSERT_TRUE(QFileInfo(filePath).exists());
        ASSERT_FALSE(QFileInfo(filePath + ".b").exists());
    }

    TEST_P(DocumentWriterTest, Write_Leaves_Binary_Sources_Open_If_Cancelled) {
        //mock files contents
        const QByteArray mockContent1(15, 1);
        QTemporaryFile e1;
        e1.open();
        e1.write(mockContent1);
        e1.close();

        //pbo file
        const QTemporaryDir temp;
        const QString filePath = temp.filePath("file.pbo");

        //pbo content structure
        PboDocument document("file.pbo");
        PboNode* n1 = document.root()->createHierarchy(PboPath("e1.txt"));
        n1->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(e1.fileName()));
        n1->binarySource->open();

        //call the method
        int count = 0;
        int expectedHitCount = GetParam();//experimental way
        DocumentWriter writer(filePath);
        writer.write(&document, [&count, expectedHitCount]() { count++; return count > expectedHitCount - 1; });

        //ensure bs left open
        ASSERT_TRUE(n1->binarySource->isOpen());
    }

    INSTANTIATE_TEST_SUITE_P(Write_Leaves_Binary_Sources_Open_If_Cancelled, DocumentWriterTest, testing::Range(1, 13));

    TEST(DocumentWriterTest, Write_Opens_Binary_Sources_After_Write) {
        //mock files contents
        const QByteArray mockContent1(15, 1);
        QTemporaryFile e1;
        e1.open();
        e1.write(mockContent1);
        e1.close();

        //pbo file
        const QTemporaryDir temp;
        const QString filePath = temp.filePath("file.pbo");

        //pbo content structure
        PboDocument document("file.pbo");
        PboNode* n1 = document.root()->createHierarchy(PboPath("e1.txt"));
        n1->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(e1.fileName()));
        n1->binarySource->open();

        //call the method
        DocumentWriter writer(filePath);
        writer.write(&document, []() { return false; });

        //ensure no junk left
        ASSERT_TRUE(n1->binarySource->isOpen());
    }

    TEST(DocumentWriterTest, Write_Replaces_Existing_File) {
        //mock files contents
        const QByteArray mockContent1(15, 1);
        QTemporaryFile e1;
        e1.open();
        e1.write(mockContent1);
        e1.close();

        //pbo file
        const QTemporaryDir temp;
        QFile existingFile(temp.filePath("file.pbo"));
        existingFile.open(QIODeviceBase::WriteOnly);
        existingFile.write(QByteArray("some content"));
        existingFile.close();

        //pbo content structure
        PboDocument document("file.pbo");
        PboNode* n1 = document.root()->createHierarchy(PboPath("e1.txt"));
        n1->binarySource = QSharedPointer<BinarySource>(new FsRawBinarySource(e1.fileName()));
        n1->binarySource->open();

        //call the method
        DocumentWriter writer(existingFile.fileName());
        writer.write(&document, []() { return false; });

        //ensure files in place
        ASSERT_TRUE(QFileInfo(existingFile.fileName()).exists());
        ASSERT_EQ(QFileInfo(existingFile.fileName()).size(), 106);//pbo written
        ASSERT_TRUE(QFileInfo(existingFile.fileName() + ".bak").exists());
        ASSERT_EQ(QFileInfo(existingFile.fileName() + ".bak").size(), 12);//the original file
    }

}
