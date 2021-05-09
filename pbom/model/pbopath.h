#pragma once

#include <QUrl>

namespace pboman3 {
    class PboPath : public QList<QString> {
    public:
        PboPath();

        explicit PboPath(std::initializer_list<QString> args);

        explicit PboPath(const QString& source);

        PboPath makeParent() const;

        PboPath makeChild(const QString& child) const;

        PboPath makeChild(const PboPath& child) const;

        QString toString() const;

        friend QDebug operator <<(QDebug debug, const PboPath& p);
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

    inline bool operator <(const PboPath& p1, const PboPath& p2) {
        const qsizetype l1 = p1.length();
        const qsizetype l2 = p2.length();
        const qsizetype lim = l1 < l2 ? l1 : l2;

        for (qsizetype i = 0; i < lim; i++) {
            const QString s1 = p1.at(i);
            const QString s2 = p2.at(i);
            if (s1 != s2) {
                return s1 < s2;
            }
        }

        return l1 < l2;
    }

    // ReSharper disable once CppInconsistentNaming
    inline size_t qHash(const PboPath& p, size_t seed) {
        return qHashRange(p.begin(), p.end(), seed);
    }
}
