#include <gtest/gtest.h>
#include "domain/documentheaders.h"
#include "domain/documentheaderstransaction.h"

namespace pboman3::domain::test {
    TEST(DocumentHeadersTest, Count_Returns_Headers_Count) {
        const DocumentHeaders headers(QList{
            QSharedPointer<DocumentHeader>(new DocumentHeader("h1", "v1")),
            QSharedPointer<DocumentHeader>(new DocumentHeader("h2", "v2")),
        });
        ASSERT_EQ(headers.count(), 2);
    }

    TEST(DocumentHeadersTest, At_Returns_Header) {
        const DocumentHeaders headers(QList{
            QSharedPointer<DocumentHeader>(new DocumentHeader("h1", "v1")),
            QSharedPointer<DocumentHeader>(new DocumentHeader("h2", "v2")),
        });

        ASSERT_EQ(headers.at(0)->name(), "h1");
        ASSERT_EQ(headers.at(0)->value(), "v1");

        ASSERT_EQ(headers.at(1)->name(), "h2");
        ASSERT_EQ(headers.at(1)->value(), "v2");
    }

    TEST(DocumentHeadersTest, Begin_End_Work) {
        DocumentHeaders headers(QList{
            QSharedPointer<DocumentHeader>(new DocumentHeader("h1", "v1")),
            QSharedPointer<DocumentHeader>(new DocumentHeader("h2", "v2")),
        });

        int count = 0;
        for (const DocumentHeader* header : headers) {
            ASSERT_FALSE(header->name().isEmpty());
            count++;
        }

        ASSERT_EQ(count, 2);
    }

    TEST(DocumentHeadersTest, HeadersChanged_Fires_If_Count_Of_Headers_Changed) {
        DocumentHeaders headers(QList{
            QSharedPointer<DocumentHeader>(new DocumentHeader("h1", "v1")),
            QSharedPointer<DocumentHeader>(new DocumentHeader("h2", "v2")),
        });

        int count = 0;
        QObject::connect(&headers, &DocumentHeaders::headersChanged, [&count]() { count++; });

        QSharedPointer<DocumentHeadersTransaction> tran = headers.beginTransaction();
        tran->add("h3", "v3");
        tran->commit();
        tran.clear();

        ASSERT_EQ(count, 1);
    }

    TEST(DocumentHeadersTest, HeadersChanged_Fires_If_Header_Changed) {
        DocumentHeaders headers(QList{
            QSharedPointer<DocumentHeader>(new DocumentHeader("h1", "v1")),
            QSharedPointer<DocumentHeader>(new DocumentHeader("h2", "v2")),
        });

        int count = 0;
        QObject::connect(&headers, &DocumentHeaders::headersChanged, [&count]() { count++; });

        QSharedPointer<DocumentHeadersTransaction> tran = headers.beginTransaction();
        tran->clear();
        tran->add("h2", "v2");
        tran->add("h1", "v1");
        tran->commit();
        tran.clear();

        ASSERT_EQ(count, 1);
    }

    TEST(DocumentHeadersTest, HeadersChanged_Not_Fires_If_Header_Replaced) {
        DocumentHeaders headers(QList{
            QSharedPointer<DocumentHeader>(new DocumentHeader("h1", "v1")),
            QSharedPointer<DocumentHeader>(new DocumentHeader("h2", "v2")),
        });

        int count = 0;
        QObject::connect(&headers, &DocumentHeaders::headersChanged, [&count]() { count++; });

        QSharedPointer<DocumentHeadersTransaction> tran = headers.beginTransaction();
        tran->clear();
        tran->add("h1", "v1");
        tran->add("h2", "v2");
        tran->commit();
        tran.clear();

        ASSERT_EQ(count, 0);
    }
}
