#include "model/headersmodel.h"
#include <gtest/gtest.h>

namespace pboman3::test {
    TEST(HeadersModelTest, SetData_Sets_Data) {
        const QSharedPointer<PboHeader> p1(new PboHeader("p1", "v1"));
        const QSharedPointer<PboHeader> p2(new PboHeader("p2", "v2"));

        HeadersModel model;
        model.setData(QList({p1, p2}));

        auto it = model.begin();
        ASSERT_EQ(*it, p1);
        ++it;
        ASSERT_EQ(*it, p2);
        ++it;
        ASSERT_EQ(it, model.end());
    }

    TEST(HeadersModelTest, SetData_Replaces_Data) {
        const QSharedPointer<PboHeader> p1(new PboHeader("p1", "v1"));
        const QSharedPointer<PboHeader> p2(new PboHeader("p2", "v2"));
        const QSharedPointer<PboHeader> p3(new PboHeader("p3", "v3"));
        const QSharedPointer<PboHeader> p4(new PboHeader("p4", "v4"));

        HeadersModel model;
        model.setData(QList({p1, p2}));
        model.setData(QList({p3, p4}));

        auto it = model.begin();
        ASSERT_EQ(*it, p3);
        ++it;
        ASSERT_EQ(*it, p4);
        ++it;
        ASSERT_EQ(it, model.end());
    }

    TEST(HeadersModelTest, SetData_Triggers_If_Data_Different) {
        const QSharedPointer<PboHeader> p1(new PboHeader("p1", "v1"));
        const QSharedPointer<PboHeader> p2(new PboHeader("p2", "v2"));
        const QSharedPointer<PboHeader> p3(new PboHeader("p3", "v3"));
        const QSharedPointer<PboHeader> p4(new PboHeader("p4", "v4"));

        HeadersModel model;
        model.setData(QList({p1, p2}));

        int count = 0;
        auto callback = [&count]() { count++; };
        QObject::connect(&model, &HeadersModel::changed, callback);

        model.setData(QList({p3, p4}));

        ASSERT_EQ(count, 1);
    }

     TEST(HeadersModelTest, SetData_Triggers_If_Data_Same) {
        const QSharedPointer<PboHeader> p1(new PboHeader("p1", "v1"));
        const QSharedPointer<PboHeader> p2(new PboHeader("p2", "v2"));
        const QSharedPointer<PboHeader> p3(new PboHeader("p1", "v1"));
        const QSharedPointer<PboHeader> p4(new PboHeader("p2", "v2"));

        HeadersModel model;
        model.setData(QList({p1, p2}));

        int count = 0;
        auto callback = [&count]() { count++; };
        QObject::connect(&model, &HeadersModel::changed, callback);

        model.setData(QList({p3, p4}));

        ASSERT_EQ(count, 0);
    }
}
