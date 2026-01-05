#include "pbopath.h"
#include <QStack>

namespace pboman3::domain {
    PboPath::PboPath()
        : QList() {
    }

    PboPath::PboPath(std::initializer_list<QString> args)
        : QList(args) {
    }

    PboPath::PboPath(const QString& source)
        : QList(splitOntoSegments(source)) {
    }

    PboPath PboPath::makeParent() const {
        if (empty()) {
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

    QStringList PboPath::splitOntoSegments(const QString& source) {
        QStringList result;
        QString word;
        auto chr = source.rbegin();
        while (chr != source.rend()) {
            if (isDirectorySeparatorChar(*chr)) {
                if (word.size() > 0) {
                    const auto next = PeekNext(chr);
                    if (next == source.rend() || isDirectorySeparatorChar(*next)) {
                        word.prepend(*chr);
                    } else {
                        result.prepend(word);
                        word.clear();
                    }
                } else {
                    word.prepend(*chr);
                }
            } else {
                word.prepend(*chr);
            }
            ++chr;
        }
        result.prepend(word);
        return result;
    }

    bool PboPath::isDirectorySeparatorChar(const QChar& chr) {
        return chr == '/' || chr == '\\';
    }

    QString::const_reverse_iterator PboPath::PeekNext(QString::const_reverse_iterator chr) {
        return ++chr;
    }

    QDebug operator<<(QDebug debug, const PboPath& p) {
        debug.nospace() << p.toString();
        return debug;
    }
}
