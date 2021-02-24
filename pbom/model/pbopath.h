#pragma once

#include <QUrl>

namespace pboman3 {
    class PboPath : public QList<QString> {
    public:
        PboPath();

        explicit PboPath(std::initializer_list<QString> args);

        explicit PboPath(const QString& source);

        explicit PboPath(const QUrl& source);

        PboPath makeParent() const;

        bool operator ==(const PboPath& other);
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
