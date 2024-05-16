#include "sanitizedstring.h"

namespace pboman3::io {
    SanitizedString::SanitizedString(const QString& text)
        : originalText_(nullptr) {
        originalText_ = &text;

        if (qsizetype firstInvalidCharIndex; needsSanitization(text, &firstInvalidCharIndex)) {
            sanitizedText_ = doSanitization(text, firstInvalidCharIndex);
        }
    }

    SanitizedString::operator const QString&() {
        return sanitizedText_.isEmpty() ? *originalText_ : sanitizedText_;
    }

    bool SanitizedString::needsSanitization(const QString& text, qsizetype* firstInvalidCharIndex) {
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

    QString SanitizedString::doSanitization(const QString& text, qsizetype firstInvalidCharIndex) {
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

    bool SanitizedString::isCharLegal(const QChar& chr) {
        if (chr == '<' || chr == '>' || chr == ':' || chr == ':' || chr == '"' || chr == '\\' || chr == '/' ||
            chr == '|' || chr == '?' || chr == '*' || chr == '{' || chr == '}') {
            return false;
        }

        const auto res = chr.isLetterOrNumber() || ((chr.isSpace() || chr.isSymbol() || chr.isPunct()) && chr.isPrint());
        return res;
    }

    QString SanitizedString::sanitizeChar(const QChar& chr) {
        return "%" + QString::number(chr.unicode(), 16);
    }
}
