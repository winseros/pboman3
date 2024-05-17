#include "sanitizedstring.h"
#include "util/filenames.h"
#include "util/numbers.h"
#include <QRandomGenerator>
#include <QCryptographicHash>

namespace pboman3::io {
    const QList<QString> SanitizedString::restrictedFileNames_ = {
            "CON", "PRN", "AUX", "NUL", "COM1", "COM2", "COM3",
            "COM4", "COM5", "COM6", "COM7", "COM8", "COM9", "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7",
            "LPT8", "LPT9"
    };

    SanitizedString::SanitizedString(const QString& text, int maxStringLength) {
        QString sanitized(text);

        if (qsizetype firstInvalidCharIndex; needsCharacterSanitization(sanitized, &firstInvalidCharIndex)) {
            sanitized = doCharacterSanitization(sanitized, firstInvalidCharIndex);
        } else if (QString keyWord; needsKeywordSanitization(sanitized, &keyWord)) {
            sanitized = doKeywordSanitization(sanitized, keyWord);
        } else if (needsWhitespaceSanitization(sanitized)) {
            sanitized = doWhitespaceSanitization(sanitized);
        }

        if (QChar ending; needsEndingSanitization(sanitized, &ending)) {
            sanitized = doEndingSanitization(sanitized, ending);
        }

        if (needsLengthSanitization(sanitized, maxStringLength)) {
            //such code should run in the beginning of the pipeline to truncate the text before the other processing
            //but unescaped binary character sequences make QString to report its length incorrectly
            //so the length sanitization works wrong if the string has binary symbols
            sanitized = doLengthSanitization(sanitized, maxStringLength);
        }

        sanitizedText_ = sanitized;
    }

    SanitizedString::operator const QString&() {
        return sanitizedText_;
    }

    bool SanitizedString::needsLengthSanitization(const QString& text, int maxStringLength) {
        return text.length() > maxStringLength;
    }

    QString SanitizedString::doLengthSanitization(const QString& text, int maxStringLength) {
        //turns "a-very-long-string"
        //into "a-very-{md5}~{number-chars-chopped}" string

        const int md5TextLength = 16 * 2;
        const int separatorCharCount = 2;

        auto stringCutLength = text.length() - maxStringLength + md5TextLength + separatorCharCount;
        auto stringCutDigits = util::NumberUtils::GetNumberOfDigits(stringCutLength);

        while (true) {
            auto stringCutDigits2 = util::NumberUtils::GetNumberOfDigits(stringCutLength + stringCutDigits);
            if (stringCutDigits2 == stringCutDigits) {
                break;
            }
            stringCutDigits = stringCutDigits2;
        }
        stringCutLength += stringCutDigits;

        const auto hashBytes = QCryptographicHash::hash(text.toUtf8(), QCryptographicHash::Algorithm::Md5);

        QString name = text.left(text.length() - stringCutLength);
        name.reserve(name.length() + md5TextLength + separatorCharCount + stringCutDigits);
        name += "-";

        for (const auto b: hashBytes) {
            name += QString::number(static_cast<unsigned char>(b), 16);
        }
        name += "~" + QString::number(stringCutLength);

        return name;
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

    bool SanitizedString::needsKeywordSanitization(const QString& text, QString* keyword) {
        const QString& fileNameWithoutExtension = util::FileNames::getFileNameWithoutExtension(text);
        const auto found = std::find_if(restrictedFileNames_.constBegin(), restrictedFileNames_.constEnd(),
                                        [&fileNameWithoutExtension](const QString& kwd) {
                                            return QString::compare(kwd, fileNameWithoutExtension,
                                                                    Qt::CaseInsensitive) == 0;
                                        });
        if (found != restrictedFileNames_.constEnd()) {
            *keyword = *found;
            return true;
        }
        return false;
    }

    QString SanitizedString::doKeywordSanitization(const QString& text, const QString& keyword) {
        QString result(text);
        const qint32 rnd = QRandomGenerator::global()->bounded(1000);
        result.insert(keyword.length(), "-" + QString::number(rnd));
        return result;
    }

    bool SanitizedString::needsWhitespaceSanitization(const QString& text) {
        return !text.trimmed().length();
    }

    QString SanitizedString::doWhitespaceSanitization(const QString& text) {
        QString result(text);
        result = result.replace(' ', "%20");
        return result;
    }

    bool SanitizedString::needsEndingSanitization(const QString& text, QChar* ending) {
        if (text.endsWith('.')) {
            *ending = '.';
            return true;
        } else if (text.endsWith(' ')) {
            *ending = ' ';
            return true;
        }
        return false;
    }

    QString SanitizedString::doEndingSanitization(const QString& text, const QChar& ending) {
        QString result = text.chopped(1);
        result.append(sanitizeChar(ending));
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
