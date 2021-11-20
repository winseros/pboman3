#include <gtest/gtest.h>
#include "util/qpointerlistiterator.h"
#include "io/pboheaderentity.h"

namespace pboman3::util::test {
    using namespace io;
    TEST(QPointerListIteratorTest, Ietrator_Works) {
        const QSharedPointer<PboHeaderEntity> h1(new PboHeaderEntity("n1", "v1"));
        const QSharedPointer<PboHeaderEntity> h2(new PboHeaderEntity("n2", "v2"));

        QList<QSharedPointer<PboHeaderEntity>> headers;
        headers.append(h1);
        headers.append(h2);

        auto it1 = QPointerListIterator(headers.data());
        auto it2 = QPointerListIterator(headers.data());
        ASSERT_EQ(it1, h1.get());
        ASSERT_EQ(it2, h1.get());
        ASSERT_EQ(it1, it2);

        ++it1;
        ++it2;
        ASSERT_EQ(it1, h2.get());
        ASSERT_EQ(it2, h2.get());
        ASSERT_EQ(it1, it2);

        --it1;
        --it2;
        ASSERT_EQ(it1, h1.get());
        ASSERT_EQ(it2, h1.get());
        ASSERT_EQ(it1, it2);

        it1 += 2;
        it2 += 2;
        ASSERT_EQ(it1, QPointerListIterator(headers.data() + headers.count()));
        ASSERT_EQ(it2, QPointerListIterator(headers.data() + headers.count()));
        ASSERT_EQ(it1, it2);

        it1 -= 2;
        it2 -= 2;
        ASSERT_EQ(it1, h1.get());
        ASSERT_EQ(it2, h1.get());
        ASSERT_EQ(it1, it2);

        auto it11 = it1++;
        auto it22 = it2++;
        ASSERT_EQ(it11, h1.get());
        ASSERT_EQ(it22, h1.get());
        ASSERT_EQ(it1, h2.get());
        ASSERT_EQ(it2, h2.get());
        ASSERT_EQ(it11, it22);
        ASSERT_EQ(it1, it2);

        it11 = it1--;
        it22 = it2--;
        ASSERT_EQ(it11, h2.get());
        ASSERT_EQ(it22, h2.get());
        ASSERT_EQ(it1, h1.get());
        ASSERT_EQ(it2, h1.get());
        ASSERT_EQ(it11, it22);
        ASSERT_EQ(it1, it2);

        it11 = it1 + 1;
        it22 = it2 + 1;
        ASSERT_EQ(it11, h2.get());
        ASSERT_EQ(it22, h2.get());
        ASSERT_EQ(it1, h1.get());
        ASSERT_EQ(it2, h1.get());
        ASSERT_EQ(it11, it22);
        ASSERT_EQ(it1, it2);

        it1 = it11 - 1;
        it2 = it22 - 1;
        ASSERT_EQ(it1, h1.get());
        ASSERT_EQ(it2, h1.get());
        ASSERT_EQ(it11, h2.get());
        ASSERT_EQ(it22, h2.get());
        ASSERT_EQ(it11, it22);
        ASSERT_EQ(it1, it2);

        ASSERT_EQ(static_cast<PboHeaderEntity*>(it1), static_cast<PboHeaderEntity*>(it2));
    }

    TEST(QPointerListConstIteratorTest, Ietrator_Works) {
        const QSharedPointer<PboHeaderEntity> h1(new PboHeaderEntity("n1", "v1"));
        const QSharedPointer<PboHeaderEntity> h2(new PboHeaderEntity("n2", "v2"));

        QList<QSharedPointer<PboHeaderEntity>> headers;
        headers.append(h1);
        headers.append(h2);

        auto it1 = QPointerListConstIterator(headers.data());
        auto it2 = QPointerListConstIterator(headers.data());
        ASSERT_EQ(it1, h1.get());
        ASSERT_EQ(it2, h1.get());
        ASSERT_EQ(it1, it2);

        ++it1;
        ++it2;
        ASSERT_EQ(it1, h2.get());
        ASSERT_EQ(it2, h2.get());
        ASSERT_EQ(it1, it2);

        --it1;
        --it2;
        ASSERT_EQ(it1, h1.get());
        ASSERT_EQ(it2, h1.get());
        ASSERT_EQ(it1, it2);

        it1 += 2;
        it2 += 2;
        ASSERT_EQ(it1, QPointerListConstIterator(headers.data() + headers.count()));
        ASSERT_EQ(it2, QPointerListConstIterator(headers.data() + headers.count()));
        ASSERT_EQ(it1, it2);

        it1 -= 2;
        it2 -= 2;
        ASSERT_EQ(it1, h1.get());
        ASSERT_EQ(it2, h1.get());
        ASSERT_EQ(it1, it2);

        auto it11 = it1++;
        auto it22 = it2++;
        ASSERT_EQ(it11, h1.get());
        ASSERT_EQ(it22, h1.get());
        ASSERT_EQ(it1, h2.get());
        ASSERT_EQ(it2, h2.get());
        ASSERT_EQ(it11, it22);
        ASSERT_EQ(it1, it2);

        it11 = it1--;
        it22 = it2--;
        ASSERT_EQ(it11, h2.get());
        ASSERT_EQ(it22, h2.get());
        ASSERT_EQ(it1, h1.get());
        ASSERT_EQ(it2, h1.get());
        ASSERT_EQ(it11, it22);
        ASSERT_EQ(it1, it2);

        it11 = it1 + 1;
        it22 = it2 + 1;
        ASSERT_EQ(it11, h2.get());
        ASSERT_EQ(it22, h2.get());
        ASSERT_EQ(it1, h1.get());
        ASSERT_EQ(it2, h1.get());
        ASSERT_EQ(it11, it22);
        ASSERT_EQ(it1, it2);

        it1 = it11 - 1;
        it2 = it22 - 1;
        ASSERT_EQ(it1, h1.get());
        ASSERT_EQ(it2, h1.get());
        ASSERT_EQ(it11, h2.get());
        ASSERT_EQ(it22, h2.get());
        ASSERT_EQ(it11, it22);
        ASSERT_EQ(it1, it2);

        ASSERT_EQ(static_cast<const PboHeaderEntity*>(it1), static_cast<const PboHeaderEntity*>(it2));
    }
}
