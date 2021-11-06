#pragma once

#include <QSharedPointer>
#include "io/pboheader.h"
#include "util/qpointerlistiterator.h"

namespace pboman3 {
    class HeadersModel : public QObject {
    Q_OBJECT
    public:
        QList<QSharedPointer<PboHeader>>::iterator begin();

        QList<QSharedPointer<PboHeader>>::iterator end();

        void setData(QList<QSharedPointer<PboHeader>> data);

    signals:
        void changed();

    private:
        QList<QSharedPointer<PboHeader>> data_;

        bool hasChanges(const QList<QSharedPointer<PboHeader>>& data) const;

        bool areDifferent(const PboHeader& h1, const PboHeader& h2) const;
    };
}
