#include <gtest/gtest.h>
#include "domain/pbonodetransaction.h"
#include "domain/validationexception.h"
#include "exception.h"

namespace pboman3::domain::test {
    TEST(PboNodeTransactionTest, Title_Returns_Title) {
        PboNode node("node.pbo", PboNodeType::Container, nullptr);

        const QSharedPointer<PboNodeTransaction> tran = node.beginTransaction();
        ASSERT_EQ(tran->title(), "node.pbo");
    }

    TEST(PboNodeTransactionTest, SetTitle_Throws_If_Committed) {
        PboNode node("node.pbo", PboNodeType::Container, nullptr);

        const QSharedPointer<PboNodeTransaction> tran = node.beginTransaction();
        tran->commit();
        ASSERT_THROW(tran->setTitle("new title"), InvalidOperationException);
    }

    TEST(PboNodeTransactionTest, SetTitle_Throws_If_Title_Emmpty) {
        PboNode node("node.pbo", PboNodeType::Container, nullptr);

        const QSharedPointer<PboNodeTransaction> tran = node.beginTransaction();
        try {
            tran->setTitle("");
            FAIL() << "Should have not reached this line";
        } catch (ValidationException& ex) {
            ASSERT_EQ(ex.message(), "The value can not be empty");
        }
    }

    TEST(PboNodeTransactionTest, SetTitle_Throws_If_Node_With_This_Name_Exists) {
        auto node = QSharedPointer<PboNode>::create(QString("node.pbo"), PboNodeType::Container, nullptr);
        node->createHierarchy(PboPath("f1.txt"));
        PboNode* f2 = node->createHierarchy(PboPath("f2.txt"));

        const QSharedPointer<PboNodeTransaction> tran = f2->beginTransaction();
        try {
            tran->setTitle("f1.txt");
            FAIL() << "Should have not reached this line";
        }
        catch (ValidationException& ex) {
            ASSERT_EQ(ex.message(), "The item with this name already exists");
        }
    }

    TEST(PboNodeTransactionTest, SetTitle_Renames_Node) {
        auto node = QSharedPointer<PboNode>::create("node.pbo", PboNodeType::Container, nullptr);
        node->createHierarchy(PboPath("f1.txt"));
        PboNode* f2 = node->createHierarchy(PboPath("f2.txt"));

        QSharedPointer<PboNodeTransaction> tran = f2->beginTransaction();
        tran->setTitle("f3.txt");
        tran->commit();
        tran.clear();

        ASSERT_EQ(f2->title(), "f3.txt");
    }
}
