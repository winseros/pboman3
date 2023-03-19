#include <gtest/gtest.h>
#include "io/documentreader.h"
#include <QFileInfo>
#include "io/pbofile.h"
#include "io/pboheaderio.h"
#include <QTemporaryFile>
#include "io/bs/pbobinarysource.h"
#include "io/jf/junkfilter.h"

namespace pboman3::io::test {
    class PredicateJunkFilter : public JunkFilter {
    public:
        explicit PredicateJunkFilter(std::function<bool(const PboNodeEntity*)> predicate)
            : predicate_(std::move(predicate)) {
        }

        bool IsJunk(const PboNodeEntity* entry) const override {
            return predicate_(entry);
        }

    private:
        std::function<bool(const PboNodeEntity*)> predicate_;
    };

    TEST(PboReaderTest, Read_Reads_File) {
        //build a mock pbo file
        QTemporaryFile t;
        t.open();

        PboFile p(t.fileName());
        p.open(QIODeviceBase::WriteOnly);
        const PboHeaderIO io(&p);

        const PboNodeEntity e0 = PboNodeEntity::makeSignature();
        const PboNodeEntity e1("f1", PboPackingMethod::Packed, 0x01010101, 0x02020202,
                               0x03030303, 5);
        const PboNodeEntity e2("f2", PboPackingMethod::Uncompressed, 0x05050505, 0x06060606,
                               0x07070707, 10);
        const PboNodeEntity e3 = PboNodeEntity::makeBoundary();

        const PboHeaderEntity h1("p1", "v1");
        const PboHeaderEntity h2("p2", "v2");
        const PboHeaderEntity h3 = PboHeaderEntity::makeBoundary();

        const QByteArray signature(20, 5);

        io.writeEntry(e0);
        io.writeHeader(h1);
        io.writeHeader(h2);
        io.writeHeader(h3);
        io.writeEntry(e1);
        io.writeEntry(e2);
        io.writeEntry(e3);
        p.write(QByteArray(e1.dataSize(), 1));
        p.write(QByteArray(e2.dataSize(), 2));
        p.write(QByteArray(1, 0)); //zero byte between data and signature
        p.write(signature);

        p.close();
        t.close();

        //call the method
        const DocumentReader reader(t.fileName(),
                                    QSharedPointer<JunkFilter>(new PredicateJunkFilter([](const PboNodeEntity*) {
                                        return false;
                                    })));
        const QSharedPointer<PboDocument> document = reader.read();

        //verify the results
        ASSERT_TRUE(document);

        ASSERT_EQ(document->headers()->count(), 2);
        ASSERT_EQ(document->headers()->at(0)->name(), "p1");
        ASSERT_EQ(document->headers()->at(0)->value(), "v1");
        ASSERT_EQ(document->headers()->at(1)->name(), "p2");
        ASSERT_EQ(document->headers()->at(1)->value(), "v2");

        QFileInfo fi(t.fileName());
        ASSERT_TRUE(document->root());
        ASSERT_EQ(document->root()->count(), 2);
        ASSERT_EQ(document->root()->title(), fi.fileName());
        ASSERT_EQ(document->root()->nodeType(), PboNodeType::Container);
        ASSERT_FALSE(document->root()->parentNode());

        ASSERT_EQ(document->root()->at(0)->title(), "f1");
        ASSERT_EQ(document->root()->at(0)->nodeType(), PboNodeType::File);
        const auto bs1 = dynamic_cast<PboBinarySource*>(document->root()->at(0)->binarySource.get());
        ASSERT_EQ(bs1->getInfo().compressed, true);
        ASSERT_EQ(bs1->getInfo().originalSize, 0x01010101);
        ASSERT_EQ(bs1->getInfo().timestamp, 0x03030303);
        ASSERT_EQ(bs1->getInfo().dataSize, 5);
        ASSERT_EQ(bs1->getInfo().dataOffset, 101); //where the actual data begins
        const auto bs2 = dynamic_cast<PboBinarySource*>(document->root()->at(1)->binarySource.get());
        ASSERT_EQ(bs2->getInfo().compressed, false);
        ASSERT_EQ(bs2->getInfo().originalSize, 0x05050505);
        ASSERT_EQ(bs2->getInfo().timestamp, 0x07070707);
        ASSERT_EQ(bs2->getInfo().dataSize, 10);
        ASSERT_EQ(bs2->getInfo().dataOffset, 106); //bs1.dataOffset + bs1.dataSize
    }

    TEST(PboReaderTest, Filters_Out_Junk) {
        //build a mock pbo file
        QTemporaryFile t;
        t.open();

        PboFile p(t.fileName());
        p.open(QIODeviceBase::WriteOnly);
        const PboHeaderIO io(&p);

        const PboNodeEntity e0 = PboNodeEntity::makeSignature();
        const PboNodeEntity e1("f1", PboPackingMethod::Packed, 0, 0, 0, 0);

        const QByteArray signature(20, 5);

        io.writeEntry(e0);
        io.writeEntry(e1);
        p.write(QByteArray(e1.dataSize(), 1));
        p.write(QByteArray(1, 0)); //zero byte between data and signature
        p.write(signature);

        p.close();
        t.close();

        //call the method
        const DocumentReader reader(t.fileName(),
                                    QSharedPointer<JunkFilter>(new PredicateJunkFilter([](const PboNodeEntity*) {
                                        return true;
                                    })));
        const QSharedPointer<PboDocument> document = reader.read();

        //verify the results
        ASSERT_TRUE(document);

        ASSERT_TRUE(document->root());
        ASSERT_EQ(document->root()->count(), 0);
    }
}
