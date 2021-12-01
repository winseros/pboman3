#include <gtest/gtest.h>
#include "domain/documentheaders.h"
#include "domain/documentheaderstransaction.h"
#include "exception.h"

namespace pboman3::domain::test {
    TEST(DocumentHeadersTransactionTest, Count_Returns_Headers_Count) {
        DocumentHeaders headers;

        const QSharedPointer<DocumentHeadersTransaction> tran = headers.beginTransaction();
        ASSERT_EQ(tran->count(), 0);

        tran->add("h1", "v1");
        ASSERT_EQ(tran->count(), 1);

        tran->add("h2", "v2");
        ASSERT_EQ(tran->count(), 2);

        tran->clear();
        ASSERT_EQ(tran->count(), 0);
    }

    TEST(DocumentHeadersTransactionTest, At_Returns_Header) {
        DocumentHeaders headers;

        const QSharedPointer<DocumentHeadersTransaction> tran = headers.beginTransaction();
        tran->add("h1", "v1");
        tran->add("h2", "v2");

        ASSERT_EQ(tran->at(0)->name(), "h1");
        ASSERT_EQ(tran->at(0)->value(), "v1");

        ASSERT_EQ(tran->at(1)->name(), "h2");
        ASSERT_EQ(tran->at(1)->value(), "v2");
    }

    TEST(DocumentHeadersTransactionTest, Add_Throws_If_Committed) {
        DocumentHeaders headers;

        const QSharedPointer<DocumentHeadersTransaction> tran = headers.beginTransaction();
        tran->commit();

        try {
            tran->add("h1", "v1");
            FAIL() << "Should have not reached this line";
        } catch (InvalidOperationException& ex) {
            ASSERT_EQ(ex.message(), "You must not modify a committed transaction.");
        }
    }

    
    TEST(DocumentHeadersTransactionTest, Clear_Throws_If_Committed) {
        DocumentHeaders headers;

        const QSharedPointer<DocumentHeadersTransaction> tran = headers.beginTransaction();
        tran->commit();

        try {
            tran->clear();
            FAIL() << "Should have not reached this line";
        } catch (InvalidOperationException& ex) {
            ASSERT_EQ(ex.message(), "You must not modify a committed transaction.");
        }
    }

    TEST(DocumentHeadersTransactionTest, Begin_End_Work) {
        DocumentHeaders headers;

        const QSharedPointer<DocumentHeadersTransaction> tran = headers.beginTransaction();
        tran->add("h1", "v1");
        tran->add("h2", "v2");

        int count = 0;
        for (const DocumentHeader* header : *tran) {
            ASSERT_FALSE(header->name().isEmpty());
            count++;
        }

        ASSERT_EQ(count, 2);
    }

    TEST(DocumentHeadersTransactionTest, Transaction_Commits) {
        DocumentHeaders headers;

        QSharedPointer<DocumentHeadersTransaction> tran = headers.beginTransaction();
        tran->add("h1", "v1");
        tran->add("h2", "v2");

        tran->commit();
        tran.clear();//explicitly clear

        ASSERT_EQ(headers.count(), 2);
        ASSERT_EQ(headers.at(0)->name(), "h1");
        ASSERT_EQ(headers.at(1)->name(), "h2");
    }

    TEST(DocumentHeadersTransactionTest, Transaction_RollsBack) {
        DocumentHeaders headers;

        QSharedPointer<DocumentHeadersTransaction> tran = headers.beginTransaction();
        tran->add("h1", "v1");
        tran->add("h2", "v2");

        //tran->commit();//no commit call!
        //tran.clear();//explicitly clear

        ASSERT_EQ(headers.count(), 0);
    }

    TEST(DocumentHeadersTransactionTest, Transaction_Does_Not_Change_Source_Data_On_Rollback) {
        DocumentHeaders headers(QList{
            QSharedPointer<DocumentHeader>(new DocumentHeader("h1", "v1")),
            QSharedPointer<DocumentHeader>(new DocumentHeader("h2", "v2")),
        });

        QSharedPointer<DocumentHeadersTransaction> tran = headers.beginTransaction();
        tran->add("h3", "v3");

        //tran->commit();//no commit call!
        tran.clear();//explicitly clear

        ASSERT_EQ(headers.count(), 2);
        ASSERT_EQ(headers.at(0)->name(), "h1");
        ASSERT_EQ(headers.at(1)->name(), "h2");
    }
}
