#pragma once

#include <QString>

namespace pboman3::io {
    class SanitizedString {
    public:
        explicit SanitizedString(const QString& text);

        operator const QString&();

    private:
        const QString* originalText_;
        QString sanitizedText_;

        static bool needsSanitization(const QString& text, qsizetype* firstInvalidCharIndex);

        static QString doSanitization(const QString& text, qsizetype firstInvalidCharIndex);

        static bool isCharLegal(const QChar& chr);

        static QString sanitizeChar(const QChar& chr);
    };
}
