#include "io/pbowriter.h"
#include <QTemporaryFile>
#include <gtest/gtest.h>
#include "io/bs/fsrawbinarysource.h"
#include <QDebug>

#include "io/pboheaderreader.h"

namespace pboman3::test {
    TEST(PboWriterTest, Write_Writes_File_With_Headers) {
        //mock files contents
        const QByteArray mockContent1(10, 1);
        QTemporaryFile e1;
        e1.open();
        e1.write(mockContent1);
        e1.close();

        const QByteArray mockContent2(10, 2);
        QTemporaryFile e2;
        e2.open();
        e2.write(mockContent2);
        e2.close();

        //pbo file
        QTemporaryFile placeholder;
        placeholder.open();
        placeholder.close();

        //pbo content structure
        PboNode root("file.pbo", PboNodeType::Container, nullptr);
        root.createHierarchy(PboPath("e1.txt"))->binarySource = QSharedPointer<BinarySource>(
            new FsRawBinarySource(e1.fileName()));
        root.createHierarchy(PboPath("f2/e2.txt"))->binarySource = QSharedPointer<BinarySource>(
            new FsRawBinarySource(e2.fileName()));

        //pbo headers
        HeadersModel headers;
        headers.setData(QList({
            QSharedPointer<PboHeader>(new PboHeader("h1", "v1")),
            QSharedPointer<PboHeader>(new PboHeader("h2", "v2"))
        }));

        //write the file
        PboWriter writer;
        writer.usePath(placeholder.fileName())
              .useRoot(&root)
              .useHeaders(&headers);

        writer.write([]() { return false; });

        //assert the result
        PboFile pbo(placeholder.fileName());
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
        QByteArray contents;
        contents.resize(mockContent1.size());
        pbo.read(contents.data(), contents.size());
        ASSERT_EQ(contents, mockContent2);

        contents.resize(mockContent2.size());
        pbo.read(contents.data(), contents.size());
        ASSERT_EQ(contents, mockContent1);

        //file has ended
        ASSERT_TRUE(pbo.atEnd());
    }
}
