#include "sanitizedstring.h"
#include "util/filenames.h"
#include <QRandomGenerator>

namespace pboman3::io {
    const QList<QString> SanitizedString::restrictedFileNames_ = {
            "CON", "PRN", "AUX", "NUL", "COM1", "COM2", "COM3",
            "COM4", "COM5", "COM6", "COM7", "COM8", "COM9", "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7",
            "LPT8", "LPT9"
    };

    SanitizedString::SanitizedString(const QString& text) {
        if (qsizetype firstInvalidCharIndex; needsCharacterSanitization(text, &firstInvalidCharIndex)) {
            sanitizedText_ = doCharacterSanitization(text, firstInvalidCharIndex);
        } else if (QString keyWord; needsKeywordSanitization(text, &keyWord)) {
            sanitizedText_ = doKeywordSanitization(text, keyWord);
        } else {
            sanitizedText_ = text;
        }
    }

    SanitizedString::operator const QString&() {
        return sanitizedText_;
    }

    bool SanitizedString::needsCharacterSanitization(const QString& text, qsizetype* firstInvalidCharIndex) {
        auto it = text.constBegin();
        while (it != text.constEnd()) {
            if (!isCharLegal(*it)) {
                *firstInvalidCharIndex = it - text.constBegin();
                return true;
            }
            ++it;
        }
        return false;
    }

    bool SanitizedString::needsKeywordSanitization(const QString& text, QString* keyword) {
        const QString& fileNameWithoutExtension = util::FileNames::getFileNameWithoutExtension(text);
        const auto found = std::find_if(restrictedFileNames_.constBegin(), restrictedFileNames_.constEnd(), [&fileNameWithoutExtension](const QString& kwd){
            return QString::compare(kwd, fileNameWithoutExtension, Qt::CaseInsensitive) == 0;
        });
        if (found != restrictedFileNames_.constEnd()){
            *keyword = *found;
            return true;
        }
        return false;
    }

    QString SanitizedString::doCharacterSanitization(const QString& text, qsizetype firstInvalidCharIndex) {
        QString sanitized;
        sanitized.reserve(static_cast<qsizetype>(static_cast<double>(text.size()) * 1.2));

        auto it = text.constBegin();
        const auto firstInvalid = it + firstInvalidCharIndex;
        while (it != firstInvalid) {
            sanitized.append(*it);
            ++it;
        }

        while (it != text.constEnd()) {
            if (isCharLegal(*it)) {
                sanitized.append(*it);
            } else {
                QString s = sanitizeChar(*it);
                sanitized.append(s);
            }
            ++it;
        }
        return sanitized;
    }

    QString SanitizedString::doKeywordSanitization(const QString& text, const QString& keyword) {
        QString result(text);
        const qint32 rnd = QRandomGenerator::global()->bounded(1000);
        result.insert(keyword.length(), "-" + QString::number(rnd));
        return result;
    }

    bool SanitizedString::isCharLegal(const QChar& chr) {
        if (chr == '<' || chr == '>' || chr == ':' || chr == ':' || chr == '"' || chr == '\\' || chr == '/' ||
            chr == '|' || chr == '?' || chr == '*' || chr == '{' || chr == '}') {
            return false;
        }

        const auto res =
                chr.isLetterOrNumber() || ((chr.isSpace() || chr.isSymbol() || chr.isPunct()) && chr.isPrint());
        return res;
    }

    QString SanitizedString::sanitizeChar(const QChar& chr) {
        return "%" + QString::number(chr.unicode(), 16);
    }
}
