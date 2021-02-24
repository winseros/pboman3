#include "pbopath.h"
#include <QRegularExpression>

namespace pboman3 {


    PboPath::PboPath()
        : QList() {
    }

    PboPath::PboPath(std::initializer_list<QString> args)
        : QList(args) {
    }

    PboPath::PboPath(const QString& source)
        : QList(source.split(QRegularExpression("\\\\|/"), Qt::SkipEmptyParts)) {
    }

    PboPath::PboPath(const QUrl& source)
        : QList(source.path().split(QRegularExpression("/"), Qt::SkipEmptyParts)) {
    }

    PboPath PboPath::makeParent() const {
        if (length() == 0) {
            return *this;
        }

        PboPath result;
        for (auto i = 0; i < length() - 1; i++) {
            result.append(this->at(i));
        }
        return result;
    }

    bool PboPath::operator==(const PboPath& other) {
        if (other.length() != length())
            return false;

        auto it1 = other.begin();
        auto it2 = begin();

        while (it1 != other.end()) {
            if (*it1 != *it2)
                return false;
            ++it1;
            ++it2;
        }

        return true;
    }
}
