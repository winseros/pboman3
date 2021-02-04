#include "io/pbofile.h"
#include <QTemporaryFile>
#include <gtest/gtest.h>

namespace pboman3::test {
    TEST(PboFileTest, ReadCString_Reads_Zero_Terminated_String) {
        static const char* mockString1 = "some string value 11";
        static const char* mockString2 = "some string value 22";

        QByteArray z;
        z.append(mockString1, 20);
        z.append(static_cast<char>(0));
        z.append(mockString2, 20);
        z.append(static_cast<char>(0));

        QTemporaryFile t;
        ASSERT_TRUE(t.open());
        t.write(z);
        t.close();

        PboFile p(t.fileName());
        p.open(QIODeviceBase::ReadOnly);

        QString s1;
        const int l1 = p.readCString(s1);
        ASSERT_EQ(s1, "some string value 11");
        ASSERT_EQ(l1, 20);
        ASSERT_EQ(p.pos(), 21);

        QString s2;
        const int l2 = p.readCString(s2);
        ASSERT_EQ(s2, "some string value 22");
        ASSERT_EQ(l2, 20);
        ASSERT_EQ(p.pos(), 42);
    }

    TEST(PboFileTest, ReadCString_Reads_Zero_Length_Strings) {
        QByteArray z;
        z.append(static_cast<char>(0));

        QTemporaryFile t;
        ASSERT_TRUE(t.open());
        t.write(z);
        t.close();

        PboFile p(t.fileName());
        p.open(QIODeviceBase::ReadOnly);

        QString s1;
        const int l1 = p.readCString(s1);
        ASSERT_TRUE(s1.isEmpty());
        ASSERT_EQ(l1, 0);
        ASSERT_EQ(p.pos(), 1);
    }

    TEST(PboFileTest, ReadCString_Not_Reads_If_No_Zero_Terminated_String) {
        static const char* mockString1 = "some string value 11";

        QByteArray z;
        z.append(mockString1, 20);
        

        QTemporaryFile t;
        qDebug(t.fileName().toStdString().c_str());
        ASSERT_TRUE(t.open());
        t.write(z);
        t.close();

        PboFile p(t.fileName());
        p.open(QIODeviceBase::ReadOnly);

        QString s1;
        const int l1 = p.readCString(s1);
        ASSERT_TRUE(s1.isEmpty());
        ASSERT_EQ(l1, 0);
        ASSERT_EQ(p.pos(), 0);
    }

    TEST(PboFileTest, WriteCString_Writes_Zero_Terminated_String) {
        QTemporaryFile t;
        ASSERT_TRUE(t.open());

        static const char* mockString1 = "some string value 11";
        static const char* mockString2 = "some string value 22";

        PboFile p(t.fileName());
        p.open(QIODeviceBase::WriteOnly);
        p.writeCString(mockString1);
        p.writeCString(mockString2);
        p.close();

        QByteArray z;
        z.append(mockString1, 20);
        z.append(static_cast<char>(0));
        z.append(mockString2, 20);
        z.append(static_cast<char>(0));

        const QByteArray d = t.readAll();

        ASSERT_EQ(t.size(), 42);
        ASSERT_EQ(d.size(), 42);
        ASSERT_EQ(z.compare(d), 0);
    }
}
