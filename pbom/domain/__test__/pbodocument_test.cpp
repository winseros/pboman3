#include "domain/pbodocument.h"
#include <gtest/gtest.h>
#include "domain/documentheaderstransaction.h"
#include "domain/pbonodetransaction.h"

namespace pboman3::domain::test {
    TEST(PboDocumentTest, Public_Ctor_Changed_Fires_When_Hierarchy_Changes) {
        const PboDocument document("file.pbo");

        int count = 0;
        QObject::connect(&document, &PboDocument::changed, [&count]() {
            count++;
        });

        document.root()->createHierarchy(PboPath("f1.txt"), ConflictResolution::Unset);

        ASSERT_EQ(count, 1);
    }

    TEST(PboDocumentTest, Repository_Ctor_Changed_Fires_When_Hierarchy_Changes) {
        const PboDocument document("file.pbo", QList<QSharedPointer<DocumentHeader>>{}, QByteArray{});

        int count = 0;
        QObject::connect(&document, &PboDocument::changed, [&count]() {
            count++;
        });

        document.root()->createHierarchy(PboPath("f1.txt"), ConflictResolution::Unset);

        ASSERT_EQ(count, 1);
    }


    TEST(PboDocumentTest, Public_Ctor_Changed_Fires_When_Headers_Change) {
        const PboDocument document("file.pbo");

        int count = 0;
        QObject::connect(&document, &PboDocument::changed, [&count]() {
            count++;
        });

        QSharedPointer<DocumentHeadersTransaction> tran = document.headers()->beginTransaction();
        tran->add("h1", "v1");
        tran->commit();
        tran.clear();

        ASSERT_EQ(count, 1);
    }

    TEST(PboDocumentTest, Repository_Ctor_Changed_Fires_When_Header_Change) {
        const PboDocument document("file.pbo", QList<QSharedPointer<DocumentHeader>>{}, QByteArray{});

        int count = 0;
        QObject::connect(&document, &PboDocument::changed, [&count]() {
            count++;
        });

        QSharedPointer<DocumentHeadersTransaction> tran = document.headers()->beginTransaction();
        tran->add("h1", "v1");
        tran->commit();
        tran.clear();

        ASSERT_EQ(count, 1);
    }


    TEST(PboDocumentTest, Public_Ctor_TitleChanged_Fires) {
        const PboDocument document("file.pbo");

        int count = 0;
        QString title;
        QObject::connect(&document, &PboDocument::titleChanged, [&count, &title](const QString& t) {
            count++;
            title = t;
        });

        QSharedPointer<PboNodeTransaction> tran = document.root()->beginTransaction();
        tran->setTitle("new.pbo");
        tran->commit();
        tran.clear();

        ASSERT_EQ(count, 1);
        ASSERT_EQ(title, "new.pbo");
    }

    TEST(PboDocumentTest, Repository_Ctor_TitleChanged_Fires) {
        const PboDocument document("file.pbo", QList<QSharedPointer<DocumentHeader>>{}, QByteArray{});

        int count = 0;
        QString title;
        QObject::connect(&document, &PboDocument::titleChanged, [&count, &title](const QString& t) {
            count++;
            title = t;
        });

        QSharedPointer<PboNodeTransaction> tran = document.root()->beginTransaction();
        tran->setTitle("new.pbo");
        tran->commit();
        tran.clear();

        ASSERT_EQ(count, 1);
        ASSERT_EQ(title, "new.pbo");
    }
}
