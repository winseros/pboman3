#pragma once

#include <QMap>

namespace pboman3 {
    class PboPath : public QList<QString> {
    public:
        PboPath makeParent() const {
            if (length() == 0) {
                return *this;
            }

            PboPath result;
            for (auto i = 0; i < length() - 1; i++) {
                result.append(this->at(i));
            }
            return result;
        }
    };

    inline bool operator ==(const PboPath& p1, const PboPath& p2) {
        if (p1.length() != p2.length())
            return false;

        auto it1 = p1.begin();
        auto it2 = p2.begin();

        while (it1 != p1.end()) {
            if (*it1 != *it2)
                return false;
            ++it1;
            ++it2;
        }

        return true;
    }

    // ReSharper disable once CppInconsistentNaming
    inline size_t qHash(const PboPath& p, size_t seed) {
        return qHashRange(p.begin(), p.end(), seed);
    }
}
