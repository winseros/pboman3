#pragma once

#include <QString>
#include <QList>

namespace pboman3::io {
    class SanitizedString {
    public:
        explicit SanitizedString(const QString& text, int maxStringLength = 255);

        operator const QString&();

    private:
        static const QList<QString> restrictedFileNames_;

        QString sanitizedText_;

        static bool needsLengthSanitization(const QString& text, int maxStringLength);

        static QString doLengthSanitization(const QString& text, int maxStringLength);

        static bool needsCharacterSanitization(const QString& text, qsizetype* firstInvalidCharIndex);

        static QString doCharacterSanitization(const QString& text, qsizetype firstInvalidCharIndex);

        static bool needsKeywordSanitization(const QString& text, QString* keyword);

        static QString doKeywordSanitization(const QString& text, const QString& keyword);

        static bool needsWhitespaceSanitization(const QString& text);

        static QString doWhitespaceSanitization(const QString& text);

        static bool needsEndingSanitization(const QString& text, QChar* ending);

        static QString doEndingSanitization(const QString& text, const QChar& ending);

        static bool isCharLegal(const QChar& chr);

        static QString sanitizeChar(const QChar& chr);
    };
}
