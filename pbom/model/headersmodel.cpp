#include "headersmodel.h"

namespace pboman3 {
    QList<QSharedPointer<PboHeader>>::iterator HeadersModel::begin() {
        return data_.begin();
    }

    QList<QSharedPointer<PboHeader>>::iterator HeadersModel::end() {
        return data_.end();
    }

    void HeadersModel::setData(QList<QSharedPointer<PboHeader>> data) {
        if (hasChanges(data)) {
            data_ = std::move(data);
            emit changed();
        }
    }

    bool HeadersModel::hasChanges(const QList<QSharedPointer<PboHeader>>& data) const {
        if (data.count() != data_.count()) {
            return true;
        }

        auto d1 = data.begin();
        auto d2 = data_.begin();

        while (d1 != data.end()) {
            if (areDifferent(**d1, **d2)) {
                return true;
            }
            ++d1;
            ++d2;
        }

        return false;
    }

    bool HeadersModel::areDifferent(const PboHeader& h1, const PboHeader& h2) const {
        return h1.name != h2.name || h1.value != h2.value;
    }
}
