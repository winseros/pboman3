#include "pbopath.h"
#include <QRegularExpression>

namespace pboman3::domain {
    PboPath::PboPath()
        : QList() {
    }

    PboPath::PboPath(std::initializer_list<QString> args)
        : QList(args) {
    }

    PboPath::PboPath(const QString& source)
        : QList(source.split(QRegularExpression("\\\\|/"), Qt::SkipEmptyParts)) {
    }

    PboPath PboPath::makeParent() const {
        if (length() == 0) {
            return *this;
        }

        PboPath result;
        result.reserve(length() - 1);
        for (auto i = 0; i < length() - 1; i++) {
            result.append(this->at(i));
        }
        return result;
    }


    PboPath PboPath::makeChild(const QString& child) const {
        PboPath result;
        result.reserve(length() + 1);
        for (const QString& s : *this)
            result.append(s);
        result.append(child);
        return result;
    }

    PboPath PboPath::makeSibling(const QString& sibling) const {
        PboPath result;
        result.reserve(length());
        for (auto i = 0; i < length() - 1; i++) {
            result.append(this->at(i));
        }
        result.append(sibling);
        return result;
    }

    QString PboPath::toString() const {
        if (count()) {
            QString path = this->at(0);
            for (auto i = 1; i < length(); i++) {
                path = path + "/" + this->at(i);
            }
            return path;
        }
        return "";
    }

    QDebug operator<<(QDebug debug, const PboPath& p) {
        debug.nospace() << p.toString();
        return debug;
    }
}
